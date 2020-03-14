/*
 * Copyright (c) 2012 Citrix Systems Inc
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <caml/mlvalues.h>
#include <caml/memory.h>
#include <caml/alloc.h>
#include <caml/fail.h>
#include <caml/bigarray.h>

#include <xen/xen.h>
#include <mini-os/os.h>
#include <xen/hvm/params.h>
#include <mini-os/kernel.h>

CAMLprim value
caml_cmdline(value v_unit)
{
  CAMLparam1(v_unit);
  CAMLreturn(caml_copy_string(&cmdline));
}

CAMLprim value
caml_console_start_page(value v_unit)
{
  CAMLparam1(v_unit);
  uint64_t console;
  ///hvm_get_parameter is missing, so construct our own call to HYPERVISOR_hvm_op
  //code copied from hypervisor.c in minios-xen
  struct xen_hvm_param xhv;
  int ret;

  xhv.domid = DOMID_SELF; //known from grant_table.h
  xhv.index = HVM_PARAM_CONSOLE_PFN;
  ret = HYPERVISOR_hvm_op(HVMOP_get_param, &xhv);
  if (ret != 0) exit(4);
  console = xhv.value;
  CAMLreturn(caml_ba_alloc_dims(CAML_BA_UINT8 | CAML_BA_C_LAYOUT,
				1,
				pfn_to_virt(console),
				(long)PAGE_SIZE));
}

CAMLprim value
caml_xenstore_start_page(value v_unit)
{
  CAMLparam1(v_unit);
  uint64_t store;
  if (hvm_get_parameter(HVM_PARAM_STORE_PFN, &store)) exit(2);
  /* FIXME: map this store page somewhere */
  CAMLreturn(caml_ba_alloc_dims(CAML_BA_UINT8 | CAML_BA_C_LAYOUT,
				1,
				(void *)pfn_to_virt(store),
				(long)PAGE_SIZE));
  CAMLreturn(v_unit);
}

CAMLprim value
caml_xenstore_event_channel(value v_unit)
{
  CAMLparam1(v_unit);
  uint64_t evtchn;
  if (hvm_get_parameter(HVM_PARAM_STORE_EVTCHN, &evtchn)) exit(3);
  CAMLreturn (Val_int(evtchn));
}
