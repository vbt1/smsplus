/*****************************************************************************
 *      デモデモソフトファイルローダ
 *
 *      Copyright (c) 1995 CSK Research Institute Corp.
 *      Copyright (c) 1995 SEGA
 *
 * Library:FLD
 * Module :終了関数
 * File   :exit.c
 * Date   :1995-08-03
 * Version:1.10
 * Auther :T.K.
 *
 *****************************************************************************/

/*****************************************************************************
 *      インクルードファイル
 *****************************************************************************/
#ifdef	__GNUC__
	#include <machine.h>
#else
	#include <machine2.h>
#endif

#include "sega_sys.h"
#include "sega_cdc.h"
#include "sega_gfs.h"
#include "sega_per.h"

/*****************************************************************************
 *      定数マクロ
 *****************************************************************************/
/* 絞り番号 */
#define FLT_READ        0       /* 作業用 */
#define FLT_DUMMY       23      /* ルートディレクトリへの移動用 */

/* タイムアウト値 */
#define TMOUT_HIRQREQ    2900000    /* HIRQREQのデフォルト :  4s */
#define TMOUT_IPLTSK    15000000    /* IP読み込みタスク    : 21s */

/* スタックポインタ */
#define STACKPTR        0x06002000

/*****************************************************************************
 *      処理マクロ
 *****************************************************************************/
/* IPロード処理（即時復帰型） */
#define SYS_IPLGAMECD(dummy) \
            ((**(Sint32 (**)(Sint32))0x600029c)(dummy))

/* IPチェック＆ラン（即時復帰型） */
#define SYS_RUNGAMECD(dummy) \
            ((**(Sint32 (**)(Sint32))0x6000288)(dummy))

/*****************************************************************************
 *      関数の宣言
 *****************************************************************************/
static void restart(void);
static void execRestart(void);
static void initSaturn(void);
static Sint32 resetCd(void);
static Sint32 waitHirq(Sint32 flag);
static void chgDirRootDummy(void);
static void execIpChkRun(void);
static Sint32 setStackptr(Sint32 addr);

/*****************************************************************************
 *      関数の定義
 *****************************************************************************/

/*
 * 終了関数
 * [入力]
 *      code : 機能コード
 *           0      : 再起動（IPチェック＆ランの実行）
 *           1      : マルチプレーヤの起動
 *           負の値 : 無限ループ
 *           その他 : 0と同じ（デフォルト）
 */
void exit(Sint32 code)
{
    switch(code) {
    case 1:     /* マルチプレーヤにジャンプ */
        SYS_EXECDMP();
        break;
    default:
        if (code < 0) {
            for(;;);        /* 負の値の場合は無限ループ */
        }
        restart();
        break;
    }
}


/* 再起動実行関数 */
static void restart(void)
{
    /* 割り込み禁止 */
    set_imask(15);

    /* 再起動の実行 */
    execRestart();

    /*
     * マルチプレーヤの起動
     * （再起動が失敗したときのみ実行される。）
     */
    SYS_EXECDMP();
}


/* 再起動実行関数 */
static void execRestart(void)
{
    Sint32  ret;
    Sint32  timer;

    /* デモデモ実行中以外ならマルチプレーヤを実行 */
    if (GFS_IS_DDS() == FALSE) {
        return;
    }

    /* マシンの初期化 */
    initSaturn();

    /* CDのソフトリセット */
    if (resetCd() != OK) {
        return;
    }

    /* IPロードの開始 */
    ret = SYS_IPLGAMECD(0);
    if (ret != OK) {
        return;
    }

    /* IPチェック＆ラン処理 */
    execIpChkRun();
}


/* マシンの初期化 */
static void initSaturn(void)
{
    /* SMPCの終了待ち＆スレーブCPUの停止 */
    PER_SMPC_SSH_OFF();

    /* 26MHzにクロックチェンジ */
    SYS_CHGSYSCK(0);
}


/* CDのソフトリセット */
static Sint32 resetCd(void)
{
    /* ソフトリセット＆終了待ち */
    if (CDC_CdInit(1, 0, 0, 0) != CDC_ERR_OK) {
        return NG;
    }
    if (waitHirq(CDC_HIRQ_ESEL) != OK) {
        return NG;
    }

    /* 形式的なルートディレクトリへの移動処理 */
    chgDirRootDummy();

    return OK;
}


/* 割り込み要因レジスタのビットが1になるまで待つ */
static Sint32 waitHirq(Sint32 flag)
{
    Sint32  i;

    for (i = 0; i < TMOUT_HIRQREQ; i++) {
        if ((CDC_GetHirqReq() & flag) != 0) {
            return OK;
        }
    }
    return NG;
}


/* 形式的なルートディレクトリへの移動処理 */
static void chgDirRootDummy(void)
{
    CDC_ChgDir(FLT_DUMMY, CDC_NUL_FID);

    CDC_AbortFile();

    return;
}


/* IPチェック＆ラン処理 */
/* （決してリターンしない関数） */
static void execIpChkRun(void)
{
    /* スタックポインタの保存 */
    static Sint32   savesp;

    register Sint32 timer;
    register Sint32 ret;

    /* スタックの設定 */
    savesp = setStackptr(STACKPTR);

    timer = 0;
    while (TRUE) {
        ret = SYS_RUNGAMECD(0);
        if (ret < 0) {
            break;
        }
        if (timer++ > TMOUT_IPLTSK) {
            break;
        }
    }

    /* スタックを戻す（不要であるが念のため） */
    setStackptr(savesp);

    /* マルチプレーヤの起動 */
    SYS_EXECDMP();
}


/*
 * スタックの設定
 * [入力]
 *      addr : 設定するスタックポインタ
 * [関数値]
 *      設定変更前のスタックポインタの値を返す
 */
/*	1995-10-02	N.K	*/
#ifdef	__GNUC__

static Sint32 setStackptr(Sint32 addr)
{
	asm("  mov       r15, r0");  
    asm("  mov       r4, r15");
}
/*
 asm volatile (													\
"add %0,%2				\n"                                     \
"mov %2,r0				\n"										\
"and #0x80,r0			\n"										\
"or r0,%1				\n"										\
"cmp/pl %2				\n"										\
*/

#else

#pragma inline_asm(setStackptr)
static Sint32 setStackptr(Sint32 addr)
{
          MOV.L       R15, R0
          MOV.L       R4, R15
}

#endif

/* end of file */

