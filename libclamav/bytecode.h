/*
 *  Load, verify and execute ClamAV bytecode.
 *
 *  Copyright (C) 2009 Sourcefire, Inc.
 *
 *  Authors: Török Edvin
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *  MA 02110-1301, USA.
 */
#ifndef BYTECODE_H
#define BYTECODE_H
#include "cltypes.h"
#include "clambc.h"
#include <stdio.h>
#include "fmap.h"

struct cli_dbio;
struct cli_bc_ctx;
struct cli_bc_func;
struct cli_bc_value;
struct cli_bc_inst;
struct cli_bc_type;
struct cli_bc_engine;
struct cli_bc_dbgnode;
struct bitset_tag;
struct cl_engine;

enum bc_state {
    bc_skip,
    bc_loaded,
    bc_jit,
    bc_interp
};

struct cli_bc {
  unsigned verifier;
  char *sigmaker;
  unsigned id;
  unsigned kind;
  struct bytecode_metadata metadata;
  unsigned num_types;
  unsigned num_func;
  struct cli_bc_func *funcs;
  struct cli_bc_type *types;
  uint64_t **globals;
  uint16_t *globaltys;
  size_t num_globals;
  enum bc_state state;
  uint16_t start_tid;
  struct bitset_tag *uses_apis;
  char *lsig;
  char *vnameprefix;
  char **vnames;
  unsigned vnames_cnt;
  struct cli_bc_dbgnode *dbgnodes;
  unsigned dbgnode_cnt;
};

struct cli_all_bc {
    struct cli_bc *all_bcs;
    unsigned count;
    struct cli_bcengine *engine;
};

struct cli_pe_hook_data;
struct cli_bc_ctx *cli_bytecode_context_alloc(void);
/* FIXME: we can't include others.h because others.h includes us...*/
void cli_bytecode_context_setctx(struct cli_bc_ctx *ctx, void *cctx);
int cli_bytecode_context_setfuncid(struct cli_bc_ctx *ctx, const struct cli_bc *bc, unsigned funcid);
int cli_bytecode_context_setparam_int(struct cli_bc_ctx *ctx, unsigned i, uint64_t c);
int cli_bytecode_context_setparam_ptr(struct cli_bc_ctx *ctx, unsigned i, void *data, unsigned datalen);
int cli_bytecode_context_setfile(struct cli_bc_ctx *ctx, fmap_t *map);
int cli_bytecode_context_setpe(struct cli_bc_ctx *ctx, const struct cli_pe_hook_data *data);
int cli_bytecode_context_clear(struct cli_bc_ctx *ctx);
/* returns file descriptor, sets tempfile. Caller takes ownership, and is
 * responsible for freeing/unlinking */
int cli_bytecode_context_getresult_file(struct cli_bc_ctx *ctx, char **tempfilename);
uint64_t cli_bytecode_context_getresult_int(struct cli_bc_ctx *ctx);
void cli_bytecode_context_destroy(struct cli_bc_ctx *ctx);

#ifdef __cplusplus
extern "C" {
#endif
extern int have_clamjit;
#ifdef __cplusplus
}
#endif
int cli_bytecode_init(struct cli_all_bc *allbc);
int cli_bytecode_load(struct cli_bc *bc, FILE *f, struct cli_dbio *dbio);
int cli_bytecode_prepare(struct cli_all_bc *allbc);
int cli_bytecode_run(const struct cli_all_bc *bcs, const struct cli_bc *bc, struct cli_bc_ctx *ctx);
void cli_bytecode_destroy(struct cli_bc *bc);
int cli_bytecode_done(struct cli_all_bc *allbc);

/* Hooks */
struct cli_exe_info;
int cli_bytecode_runlsig(const struct cli_all_bc *bcs, const struct cli_bc* bc, const char **virname, const uint32_t* lsigcnt, fmap_t *fmap);
int cli_bytecode_runhook(const struct cl_engine *engine, struct cli_bc_ctx *ctx, unsigned id, fmap_t *map, const char **virname);

#ifdef __cplusplus
extern "C" {
#endif

int bytecode_init(void);
/* Bytecode internal debug API */
void cli_bytecode_debug(int argc, char **argv);
void cli_bytecode_printversion();
void cli_bytecode_debug_printsrc(const struct cli_bc_ctx *ctx);

typedef void (*bc_dbg_callback_trace)(struct cli_bc_ctx*, unsigned event);
typedef void (*bc_dbg_callback_trace_op)(struct cli_bc_ctx*, const char *op);
typedef void (*bc_dbg_callback_trace_val)(struct cli_bc_ctx*, const char *name, uint32_t value);
typedef void (*bc_dbg_callback_trace_ptr)(struct cli_bc_ctx*, const void *val);
void cli_bytecode_context_set_trace(struct cli_bc_ctx*, unsigned level,
				    bc_dbg_callback_trace,
				    bc_dbg_callback_trace_op,
				    bc_dbg_callback_trace_val,
				    bc_dbg_callback_trace_ptr);

#ifdef __cplusplus
}
#endif

#endif
