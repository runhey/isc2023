/***************************************************************************************
 * Copyright (c) 2014-2022 Zihao Yu, Nanjing University
 *
 * NEMU is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 *
 * See the Mulan PSL v2 for more details.
 ***************************************************************************************/

#include <dlfcn.h>

#include <isa.h>
#include <cpu/cpu.h>
#include <memory/paddr.h>
#include <utils.h>
#include <difftest-def.h>

void (*ref_difftest_memcpy)(paddr_t addr, void *buf, size_t n, bool direction) = NULL;
void (*ref_difftest_regcpy)(void *dut, bool direction) = NULL;
void (*ref_difftest_exec)(uint64_t n) = NULL;
void (*ref_difftest_raise_intr)(uint64_t NO) = NULL;

#define CONFIG_DIFFTEST

#ifdef CONFIG_DIFFTEST

static bool is_skip_ref = false;
static int skip_dut_nr_inst = 0;

// this is used to let ref skip instructions which
// can not produce consistent behavior with NEMU
// 这用于让ref跳过不能与NEM产生一致行为的指令
void difftest_skip_ref()
{
  is_skip_ref = true;
  // If such an instruction is one of the instruction packing in QEMU
  // (see below), we end the process of catching up with QEMU's pc to
  // keep the consistent behavior in our best.
  // Note that this is still not perfect: if the packed instructions
  // already write some memory, and the incoming instruction in NEMU
  // will load that memory, we will encounter false negative. But such
  // situation is infrequent.
  // is_skip_ref 是一个标志位，用于指示模拟器（NEMU）是否应该跳过与参考设计（DUT）之间的比较。
  // 当 is_skip_ref 为真时，模拟器将跳过与参考设计之间的比较，直到 is_skip_ref 的值为假。
  // 这个标志位的目的是在某些情况下跳过与参考设计之间的比较，例如当参考设计执行了多个指令来模拟一个 QEMU 指令时。
  // 在这种情况下，我们可以跳过与参考设计之间的比较，直到模拟器的状态与参考设计的状态相匹配，从而保持一致的行为。

  skip_dut_nr_inst = 0;
  // skip_dut_nr_inst 是一个计数器，用于跟踪模拟器（NEMU）跳过与参考设计（DUT）进行比较的指令数量。
  // 当模拟器与参考设计之间存在指令打包时，可能会出现这种情况，即一次 QEMU 指令可能执行多次模拟器指令。
  // 在这种情况下，我们需要跳过一些模拟器指令，直到模拟器的状态与 QEMU 的状态相匹配，从而保持一致的行为。
  // skip_dut_nr_inst 的作用就是记录模拟器需要跳过的指令数量。当 skip_dut_nr_inst 的值大于 0 时，
  // 模拟器将跳过接下来的 skip_dut_nr_inst 指令，直到 skip_dut_nr_inst 的值为 0。
}

// this is used to deal with instruction packing in QEMU.
// Sometimes letting QEMU step once will execute multiple instructions.
// We should skip checking until NEMU's pc catches up with QEMU's pc.
// The semantic is
//   Let REF run `nr_ref` instructions first.
//   We expect that DUT will catch up with REF within `nr_dut` instructions.
// 在这种情况下，我们允许QEMU执行一次指令，然后暂停模拟器，直到模拟器的状态与QEMU的状态相匹配，从而保持一致的行为。
// 这个概念被称为“skip checking until NEMU's pc catches up with QEMU's pc”。
void difftest_skip_dut(int nr_ref, int nr_dut)
{
  skip_dut_nr_inst += nr_dut;

  while (nr_ref-- > 0)
  {
    ref_difftest_exec(1);
  }
}

void init_difftest(char *ref_so_file, long img_size, int port)
{
  assert(ref_so_file != NULL);

  void *handle;
  handle = dlopen(ref_so_file, RTLD_LAZY);
  assert(handle);

  ref_difftest_memcpy = dlsym(handle, "difftest_memcpy");
  assert(ref_difftest_memcpy);

  ref_difftest_regcpy = dlsym(handle, "difftest_regcpy");
  assert(ref_difftest_regcpy);

  ref_difftest_exec = dlsym(handle, "difftest_exec");
  assert(ref_difftest_exec);

  ref_difftest_raise_intr = dlsym(handle, "difftest_raise_intr");
  assert(ref_difftest_raise_intr);

  void (*ref_difftest_init)(int) = dlsym(handle, "difftest_init");
  assert(ref_difftest_init);

  Log("Differential testing: %s", ANSI_FMT("ON", ANSI_FG_GREEN));
  Log("The result of every instruction will be compared with %s. "
      "This will help you a lot for debugging, but also significantly reduce the performance. "
      "If it is not necessary, you can turn it off in menuconfig.",
      ref_so_file);

  ref_difftest_init(port);
  ref_difftest_memcpy(RESET_VECTOR, guest_to_host(RESET_VECTOR), img_size, DIFFTEST_TO_REF);
  ref_difftest_regcpy(&cpu, DIFFTEST_TO_REF);
}

// 检查寄存器的状态
static void checkregs(CPU_state *ref, vaddr_t pc)
{
  if (!isa_difftest_checkregs(ref, pc))
  {
    nemu_state.state = NEMU_ABORT;
    nemu_state.halt_pc = pc;
    isa_reg_display();
  }
}

void difftest_step(vaddr_t pc, vaddr_t npc)
{
  CPU_state ref_r;

  if (skip_dut_nr_inst > 0)
  {
    ref_difftest_regcpy(&ref_r, DIFFTEST_TO_DUT);
    if (ref_r.pc == npc)
    {
      skip_dut_nr_inst = 0;
      checkregs(&ref_r, npc);
      return;
    }
    skip_dut_nr_inst--;
    if (skip_dut_nr_inst == 0)
      panic("can not catch up with ref.pc = " FMT_WORD " at pc = " FMT_WORD, ref_r.pc, pc);
    return;
  }

  if (is_skip_ref)
  {
    // to skip the checking of an instruction, just copy the reg state to reference design
    ref_difftest_regcpy(&cpu, DIFFTEST_TO_REF);
    is_skip_ref = false;
    return;
  }

  ref_difftest_exec(1);
  ref_difftest_regcpy(&ref_r, DIFFTEST_TO_DUT);

  checkregs(&ref_r, pc);
}
#else
void init_difftest(char *ref_so_file, long img_size, int port) {}
#endif
