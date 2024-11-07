/*
 * Copyright (c) - All Rights Reserved.
 *
 * See the LICENSE file for more information.
 */

#include <kernel/hal/arch/x64/idt/isr.h>
#include <kernel/hal/arch/x64/idt/idt.h>
#include <common/io/regs.h>

extern "C" void isrHandler0(io::Registers* regs);
extern "C" void isrHandler1(io::Registers* regs);
extern "C" void isrHandler2(io::Registers* regs);
extern "C" void isrHandler3(io::Registers* regs);
extern "C" void isrHandler4(io::Registers* regs);
extern "C" void isrHandler5(io::Registers* regs);
extern "C" void isrHandler6(io::Registers* regs);
extern "C" void isrHandler7(io::Registers* regs);
extern "C" void isrHandler8(io::Registers* regs);
extern "C" void isrHandler9(io::Registers* regs);
extern "C" void isrHandler10(io::Registers* regs);
extern "C" void isrHandler11(io::Registers* regs);
extern "C" void isrHandler12(io::Registers* regs);
extern "C" void isrHandler13(io::Registers* regs);
extern "C" void isrHandler14(io::Registers* regs);
extern "C" void isrHandler15(io::Registers* regs);
extern "C" void isrHandler16(io::Registers* regs);
extern "C" void isrHandler17(io::Registers* regs);
extern "C" void isrHandler18(io::Registers* regs);
extern "C" void isrHandler19(io::Registers* regs);
extern "C" void isrHandler20(io::Registers* regs);
extern "C" void isrHandler21(io::Registers* regs);
extern "C" void isrHandler22(io::Registers* regs);
extern "C" void isrHandler23(io::Registers* regs);
extern "C" void isrHandler24(io::Registers* regs);
extern "C" void isrHandler25(io::Registers* regs);
extern "C" void isrHandler26(io::Registers* regs);
extern "C" void isrHandler27(io::Registers* regs);
extern "C" void isrHandler28(io::Registers* regs);
extern "C" void isrHandler29(io::Registers* regs);
extern "C" void isrHandler30(io::Registers* regs);
extern "C" void isrHandler31(io::Registers* regs);
extern "C" void isrHandler32(io::Registers* regs);
extern "C" void isrHandler33(io::Registers* regs);
extern "C" void isrHandler34(io::Registers* regs);
extern "C" void isrHandler35(io::Registers* regs);
extern "C" void isrHandler36(io::Registers* regs);
extern "C" void isrHandler37(io::Registers* regs);
extern "C" void isrHandler38(io::Registers* regs);
extern "C" void isrHandler39(io::Registers* regs);
extern "C" void isrHandler40(io::Registers* regs);
extern "C" void isrHandler41(io::Registers* regs);
extern "C" void isrHandler42(io::Registers* regs);
extern "C" void isrHandler43(io::Registers* regs);
extern "C" void isrHandler44(io::Registers* regs);
extern "C" void isrHandler45(io::Registers* regs);
extern "C" void isrHandler46(io::Registers* regs);
extern "C" void isrHandler47(io::Registers* regs);
extern "C" void isrHandler48(io::Registers* regs);
extern "C" void isrHandler49(io::Registers* regs);
extern "C" void isrHandler50(io::Registers* regs);
extern "C" void isrHandler51(io::Registers* regs);
extern "C" void isrHandler52(io::Registers* regs);
extern "C" void isrHandler53(io::Registers* regs);
extern "C" void isrHandler54(io::Registers* regs);
extern "C" void isrHandler55(io::Registers* regs);
extern "C" void isrHandler56(io::Registers* regs);
extern "C" void isrHandler57(io::Registers* regs);
extern "C" void isrHandler58(io::Registers* regs);
extern "C" void isrHandler59(io::Registers* regs);
extern "C" void isrHandler60(io::Registers* regs);
extern "C" void isrHandler61(io::Registers* regs);
extern "C" void isrHandler62(io::Registers* regs);
extern "C" void isrHandler63(io::Registers* regs);
extern "C" void isrHandler64(io::Registers* regs);
extern "C" void isrHandler65(io::Registers* regs);
extern "C" void isrHandler66(io::Registers* regs);
extern "C" void isrHandler67(io::Registers* regs);
extern "C" void isrHandler68(io::Registers* regs);
extern "C" void isrHandler69(io::Registers* regs);
extern "C" void isrHandler70(io::Registers* regs);
extern "C" void isrHandler71(io::Registers* regs);
extern "C" void isrHandler72(io::Registers* regs);
extern "C" void isrHandler73(io::Registers* regs);
extern "C" void isrHandler74(io::Registers* regs);
extern "C" void isrHandler75(io::Registers* regs);
extern "C" void isrHandler76(io::Registers* regs);
extern "C" void isrHandler77(io::Registers* regs);
extern "C" void isrHandler78(io::Registers* regs);
extern "C" void isrHandler79(io::Registers* regs);
extern "C" void isrHandler80(io::Registers* regs);
extern "C" void isrHandler81(io::Registers* regs);
extern "C" void isrHandler82(io::Registers* regs);
extern "C" void isrHandler83(io::Registers* regs);
extern "C" void isrHandler84(io::Registers* regs);
extern "C" void isrHandler85(io::Registers* regs);
extern "C" void isrHandler86(io::Registers* regs);
extern "C" void isrHandler87(io::Registers* regs);
extern "C" void isrHandler88(io::Registers* regs);
extern "C" void isrHandler89(io::Registers* regs);
extern "C" void isrHandler90(io::Registers* regs);
extern "C" void isrHandler91(io::Registers* regs);
extern "C" void isrHandler92(io::Registers* regs);
extern "C" void isrHandler93(io::Registers* regs);
extern "C" void isrHandler94(io::Registers* regs);
extern "C" void isrHandler95(io::Registers* regs);
extern "C" void isrHandler96(io::Registers* regs);
extern "C" void isrHandler97(io::Registers* regs);
extern "C" void isrHandler98(io::Registers* regs);
extern "C" void isrHandler99(io::Registers* regs);
extern "C" void isrHandler100(io::Registers* regs);
extern "C" void isrHandler101(io::Registers* regs);
extern "C" void isrHandler102(io::Registers* regs);
extern "C" void isrHandler103(io::Registers* regs);
extern "C" void isrHandler104(io::Registers* regs);
extern "C" void isrHandler105(io::Registers* regs);
extern "C" void isrHandler106(io::Registers* regs);
extern "C" void isrHandler107(io::Registers* regs);
extern "C" void isrHandler108(io::Registers* regs);
extern "C" void isrHandler109(io::Registers* regs);
extern "C" void isrHandler110(io::Registers* regs);
extern "C" void isrHandler111(io::Registers* regs);
extern "C" void isrHandler112(io::Registers* regs);
extern "C" void isrHandler113(io::Registers* regs);
extern "C" void isrHandler114(io::Registers* regs);
extern "C" void isrHandler115(io::Registers* regs);
extern "C" void isrHandler116(io::Registers* regs);
extern "C" void isrHandler117(io::Registers* regs);
extern "C" void isrHandler118(io::Registers* regs);
extern "C" void isrHandler119(io::Registers* regs);
extern "C" void isrHandler120(io::Registers* regs);
extern "C" void isrHandler121(io::Registers* regs);
extern "C" void isrHandler122(io::Registers* regs);
extern "C" void isrHandler123(io::Registers* regs);
extern "C" void isrHandler124(io::Registers* regs);
extern "C" void isrHandler125(io::Registers* regs);
extern "C" void isrHandler126(io::Registers* regs);
extern "C" void isrHandler127(io::Registers* regs);
extern "C" void isrHandler128(io::Registers* regs);
extern "C" void isrHandler129(io::Registers* regs);
extern "C" void isrHandler130(io::Registers* regs);
extern "C" void isrHandler131(io::Registers* regs);
extern "C" void isrHandler132(io::Registers* regs);
extern "C" void isrHandler133(io::Registers* regs);
extern "C" void isrHandler134(io::Registers* regs);
extern "C" void isrHandler135(io::Registers* regs);
extern "C" void isrHandler136(io::Registers* regs);
extern "C" void isrHandler137(io::Registers* regs);
extern "C" void isrHandler138(io::Registers* regs);
extern "C" void isrHandler139(io::Registers* regs);
extern "C" void isrHandler140(io::Registers* regs);
extern "C" void isrHandler141(io::Registers* regs);
extern "C" void isrHandler142(io::Registers* regs);
extern "C" void isrHandler143(io::Registers* regs);
extern "C" void isrHandler144(io::Registers* regs);
extern "C" void isrHandler145(io::Registers* regs);
extern "C" void isrHandler146(io::Registers* regs);
extern "C" void isrHandler147(io::Registers* regs);
extern "C" void isrHandler148(io::Registers* regs);
extern "C" void isrHandler149(io::Registers* regs);
extern "C" void isrHandler150(io::Registers* regs);
extern "C" void isrHandler151(io::Registers* regs);
extern "C" void isrHandler152(io::Registers* regs);
extern "C" void isrHandler153(io::Registers* regs);
extern "C" void isrHandler154(io::Registers* regs);
extern "C" void isrHandler155(io::Registers* regs);
extern "C" void isrHandler156(io::Registers* regs);
extern "C" void isrHandler157(io::Registers* regs);
extern "C" void isrHandler158(io::Registers* regs);
extern "C" void isrHandler159(io::Registers* regs);
extern "C" void isrHandler160(io::Registers* regs);
extern "C" void isrHandler161(io::Registers* regs);
extern "C" void isrHandler162(io::Registers* regs);
extern "C" void isrHandler163(io::Registers* regs);
extern "C" void isrHandler164(io::Registers* regs);
extern "C" void isrHandler165(io::Registers* regs);
extern "C" void isrHandler166(io::Registers* regs);
extern "C" void isrHandler167(io::Registers* regs);
extern "C" void isrHandler168(io::Registers* regs);
extern "C" void isrHandler169(io::Registers* regs);
extern "C" void isrHandler170(io::Registers* regs);
extern "C" void isrHandler171(io::Registers* regs);
extern "C" void isrHandler172(io::Registers* regs);
extern "C" void isrHandler173(io::Registers* regs);
extern "C" void isrHandler174(io::Registers* regs);
extern "C" void isrHandler175(io::Registers* regs);
extern "C" void isrHandler176(io::Registers* regs);
extern "C" void isrHandler177(io::Registers* regs);
extern "C" void isrHandler178(io::Registers* regs);
extern "C" void isrHandler179(io::Registers* regs);
extern "C" void isrHandler180(io::Registers* regs);
extern "C" void isrHandler181(io::Registers* regs);
extern "C" void isrHandler182(io::Registers* regs);
extern "C" void isrHandler183(io::Registers* regs);
extern "C" void isrHandler184(io::Registers* regs);
extern "C" void isrHandler185(io::Registers* regs);
extern "C" void isrHandler186(io::Registers* regs);
extern "C" void isrHandler187(io::Registers* regs);
extern "C" void isrHandler188(io::Registers* regs);
extern "C" void isrHandler189(io::Registers* regs);
extern "C" void isrHandler190(io::Registers* regs);
extern "C" void isrHandler191(io::Registers* regs);
extern "C" void isrHandler192(io::Registers* regs);
extern "C" void isrHandler193(io::Registers* regs);
extern "C" void isrHandler194(io::Registers* regs);
extern "C" void isrHandler195(io::Registers* regs);
extern "C" void isrHandler196(io::Registers* regs);
extern "C" void isrHandler197(io::Registers* regs);
extern "C" void isrHandler198(io::Registers* regs);
extern "C" void isrHandler199(io::Registers* regs);
extern "C" void isrHandler200(io::Registers* regs);
extern "C" void isrHandler201(io::Registers* regs);
extern "C" void isrHandler202(io::Registers* regs);
extern "C" void isrHandler203(io::Registers* regs);
extern "C" void isrHandler204(io::Registers* regs);
extern "C" void isrHandler205(io::Registers* regs);
extern "C" void isrHandler206(io::Registers* regs);
extern "C" void isrHandler207(io::Registers* regs);
extern "C" void isrHandler208(io::Registers* regs);
extern "C" void isrHandler209(io::Registers* regs);
extern "C" void isrHandler210(io::Registers* regs);
extern "C" void isrHandler211(io::Registers* regs);
extern "C" void isrHandler212(io::Registers* regs);
extern "C" void isrHandler213(io::Registers* regs);
extern "C" void isrHandler214(io::Registers* regs);
extern "C" void isrHandler215(io::Registers* regs);
extern "C" void isrHandler216(io::Registers* regs);
extern "C" void isrHandler217(io::Registers* regs);
extern "C" void isrHandler218(io::Registers* regs);
extern "C" void isrHandler219(io::Registers* regs);
extern "C" void isrHandler220(io::Registers* regs);
extern "C" void isrHandler221(io::Registers* regs);
extern "C" void isrHandler222(io::Registers* regs);
extern "C" void isrHandler223(io::Registers* regs);
extern "C" void isrHandler224(io::Registers* regs);
extern "C" void isrHandler225(io::Registers* regs);
extern "C" void isrHandler226(io::Registers* regs);
extern "C" void isrHandler227(io::Registers* regs);
extern "C" void isrHandler228(io::Registers* regs);
extern "C" void isrHandler229(io::Registers* regs);
extern "C" void isrHandler230(io::Registers* regs);
extern "C" void isrHandler231(io::Registers* regs);
extern "C" void isrHandler232(io::Registers* regs);
extern "C" void isrHandler233(io::Registers* regs);
extern "C" void isrHandler234(io::Registers* regs);
extern "C" void isrHandler235(io::Registers* regs);
extern "C" void isrHandler236(io::Registers* regs);
extern "C" void isrHandler237(io::Registers* regs);
extern "C" void isrHandler238(io::Registers* regs);
extern "C" void isrHandler239(io::Registers* regs);
extern "C" void isrHandler240(io::Registers* regs);
extern "C" void isrHandler241(io::Registers* regs);
extern "C" void isrHandler242(io::Registers* regs);
extern "C" void isrHandler243(io::Registers* regs);
extern "C" void isrHandler244(io::Registers* regs);
extern "C" void isrHandler245(io::Registers* regs);
extern "C" void isrHandler246(io::Registers* regs);
extern "C" void isrHandler247(io::Registers* regs);
extern "C" void isrHandler248(io::Registers* regs);
extern "C" void isrHandler249(io::Registers* regs);
extern "C" void isrHandler250(io::Registers* regs);
extern "C" void isrHandler251(io::Registers* regs);
extern "C" void isrHandler252(io::Registers* regs);
extern "C" void isrHandler253(io::Registers* regs);
extern "C" void isrHandler254(io::Registers* regs);
extern "C" void isrHandler255(io::Registers* regs);
namespace hal::arch::x64::idt{
    void initGates(){
        registerHandler(0, (void*)isrHandler0, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(1, (void*)isrHandler1, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(2, (void*)isrHandler2, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(3, (void*)isrHandler3, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(4, (void*)isrHandler4, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(5, (void*)isrHandler5, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(6, (void*)isrHandler6, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(7, (void*)isrHandler7, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(8, (void*)isrHandler8, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(9, (void*)isrHandler9, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(10, (void*)isrHandler10, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(11, (void*)isrHandler11, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(12, (void*)isrHandler12, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(13, (void*)isrHandler13, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(14, (void*)isrHandler14, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(15, (void*)isrHandler15, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(16, (void*)isrHandler16, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(17, (void*)isrHandler17, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(18, (void*)isrHandler18, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(19, (void*)isrHandler19, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(20, (void*)isrHandler20, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(21, (void*)isrHandler21, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(22, (void*)isrHandler22, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(23, (void*)isrHandler23, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(24, (void*)isrHandler24, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(25, (void*)isrHandler25, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(26, (void*)isrHandler26, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(27, (void*)isrHandler27, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(28, (void*)isrHandler28, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(29, (void*)isrHandler29, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(30, (void*)isrHandler30, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(31, (void*)isrHandler31, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(32, (void*)isrHandler32, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(33, (void*)isrHandler33, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(34, (void*)isrHandler34, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(35, (void*)isrHandler35, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(36, (void*)isrHandler36, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(37, (void*)isrHandler37, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(38, (void*)isrHandler38, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(39, (void*)isrHandler39, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(40, (void*)isrHandler40, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(41, (void*)isrHandler41, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(42, (void*)isrHandler42, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(43, (void*)isrHandler43, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(44, (void*)isrHandler44, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(45, (void*)isrHandler45, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(46, (void*)isrHandler46, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(47, (void*)isrHandler47, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(48, (void*)isrHandler48, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(49, (void*)isrHandler49, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(50, (void*)isrHandler50, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(51, (void*)isrHandler51, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(52, (void*)isrHandler52, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(53, (void*)isrHandler53, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(54, (void*)isrHandler54, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(55, (void*)isrHandler55, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(56, (void*)isrHandler56, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(57, (void*)isrHandler57, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(58, (void*)isrHandler58, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(59, (void*)isrHandler59, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(60, (void*)isrHandler60, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(61, (void*)isrHandler61, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(62, (void*)isrHandler62, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(63, (void*)isrHandler63, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(64, (void*)isrHandler64, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(65, (void*)isrHandler65, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(66, (void*)isrHandler66, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(67, (void*)isrHandler67, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(68, (void*)isrHandler68, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(69, (void*)isrHandler69, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(70, (void*)isrHandler70, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(71, (void*)isrHandler71, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(72, (void*)isrHandler72, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(73, (void*)isrHandler73, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(74, (void*)isrHandler74, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(75, (void*)isrHandler75, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(76, (void*)isrHandler76, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(77, (void*)isrHandler77, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(78, (void*)isrHandler78, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(79, (void*)isrHandler79, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(80, (void*)isrHandler80, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(81, (void*)isrHandler81, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(82, (void*)isrHandler82, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(83, (void*)isrHandler83, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(84, (void*)isrHandler84, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(85, (void*)isrHandler85, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(86, (void*)isrHandler86, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(87, (void*)isrHandler87, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(88, (void*)isrHandler88, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(89, (void*)isrHandler89, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(90, (void*)isrHandler90, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(91, (void*)isrHandler91, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(92, (void*)isrHandler92, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(93, (void*)isrHandler93, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(94, (void*)isrHandler94, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(95, (void*)isrHandler95, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(96, (void*)isrHandler96, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(97, (void*)isrHandler97, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(98, (void*)isrHandler98, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(99, (void*)isrHandler99, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(100, (void*)isrHandler100, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(101, (void*)isrHandler101, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(102, (void*)isrHandler102, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(103, (void*)isrHandler103, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(104, (void*)isrHandler104, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(105, (void*)isrHandler105, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(106, (void*)isrHandler106, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(107, (void*)isrHandler107, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(108, (void*)isrHandler108, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(109, (void*)isrHandler109, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(110, (void*)isrHandler110, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(111, (void*)isrHandler111, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(112, (void*)isrHandler112, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(113, (void*)isrHandler113, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(114, (void*)isrHandler114, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(115, (void*)isrHandler115, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(116, (void*)isrHandler116, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(117, (void*)isrHandler117, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(118, (void*)isrHandler118, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(119, (void*)isrHandler119, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(120, (void*)isrHandler120, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(121, (void*)isrHandler121, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(122, (void*)isrHandler122, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(123, (void*)isrHandler123, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(124, (void*)isrHandler124, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(125, (void*)isrHandler125, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(126, (void*)isrHandler126, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(127, (void*)isrHandler127, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(128, (void*)isrHandler128, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(129, (void*)isrHandler129, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(130, (void*)isrHandler130, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(131, (void*)isrHandler131, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(132, (void*)isrHandler132, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(133, (void*)isrHandler133, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(134, (void*)isrHandler134, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(135, (void*)isrHandler135, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(136, (void*)isrHandler136, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(137, (void*)isrHandler137, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(138, (void*)isrHandler138, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(139, (void*)isrHandler139, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(140, (void*)isrHandler140, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(141, (void*)isrHandler141, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(142, (void*)isrHandler142, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(143, (void*)isrHandler143, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(144, (void*)isrHandler144, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(145, (void*)isrHandler145, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(146, (void*)isrHandler146, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(147, (void*)isrHandler147, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(148, (void*)isrHandler148, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(149, (void*)isrHandler149, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(150, (void*)isrHandler150, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(151, (void*)isrHandler151, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(152, (void*)isrHandler152, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(153, (void*)isrHandler153, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(154, (void*)isrHandler154, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(155, (void*)isrHandler155, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(156, (void*)isrHandler156, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(157, (void*)isrHandler157, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(158, (void*)isrHandler158, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(159, (void*)isrHandler159, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(160, (void*)isrHandler160, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(161, (void*)isrHandler161, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(162, (void*)isrHandler162, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(163, (void*)isrHandler163, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(164, (void*)isrHandler164, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(165, (void*)isrHandler165, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(166, (void*)isrHandler166, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(167, (void*)isrHandler167, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(168, (void*)isrHandler168, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(169, (void*)isrHandler169, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(170, (void*)isrHandler170, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(171, (void*)isrHandler171, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(172, (void*)isrHandler172, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(173, (void*)isrHandler173, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(174, (void*)isrHandler174, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(175, (void*)isrHandler175, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(176, (void*)isrHandler176, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(177, (void*)isrHandler177, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(178, (void*)isrHandler178, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(179, (void*)isrHandler179, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(180, (void*)isrHandler180, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(181, (void*)isrHandler181, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(182, (void*)isrHandler182, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(183, (void*)isrHandler183, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(184, (void*)isrHandler184, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(185, (void*)isrHandler185, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(186, (void*)isrHandler186, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(187, (void*)isrHandler187, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(188, (void*)isrHandler188, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(189, (void*)isrHandler189, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(190, (void*)isrHandler190, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(191, (void*)isrHandler191, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(192, (void*)isrHandler192, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(193, (void*)isrHandler193, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(194, (void*)isrHandler194, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(195, (void*)isrHandler195, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(196, (void*)isrHandler196, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(197, (void*)isrHandler197, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(198, (void*)isrHandler198, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(199, (void*)isrHandler199, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(200, (void*)isrHandler200, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(201, (void*)isrHandler201, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(202, (void*)isrHandler202, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(203, (void*)isrHandler203, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(204, (void*)isrHandler204, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(205, (void*)isrHandler205, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(206, (void*)isrHandler206, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(207, (void*)isrHandler207, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(208, (void*)isrHandler208, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(209, (void*)isrHandler209, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(210, (void*)isrHandler210, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(211, (void*)isrHandler211, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(212, (void*)isrHandler212, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(213, (void*)isrHandler213, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(214, (void*)isrHandler214, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(215, (void*)isrHandler215, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(216, (void*)isrHandler216, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(217, (void*)isrHandler217, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(218, (void*)isrHandler218, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(219, (void*)isrHandler219, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(220, (void*)isrHandler220, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(221, (void*)isrHandler221, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(222, (void*)isrHandler222, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(223, (void*)isrHandler223, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(224, (void*)isrHandler224, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(225, (void*)isrHandler225, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(226, (void*)isrHandler226, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(227, (void*)isrHandler227, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(228, (void*)isrHandler228, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(229, (void*)isrHandler229, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(230, (void*)isrHandler230, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(231, (void*)isrHandler231, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(232, (void*)isrHandler232, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(233, (void*)isrHandler233, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(234, (void*)isrHandler234, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(235, (void*)isrHandler235, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(236, (void*)isrHandler236, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(237, (void*)isrHandler237, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(238, (void*)isrHandler238, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(239, (void*)isrHandler239, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(240, (void*)isrHandler240, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(241, (void*)isrHandler241, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(242, (void*)isrHandler242, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(243, (void*)isrHandler243, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(244, (void*)isrHandler244, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(245, (void*)isrHandler245, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(246, (void*)isrHandler246, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(247, (void*)isrHandler247, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(248, (void*)isrHandler248, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(249, (void*)isrHandler249, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(250, (void*)isrHandler250, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(251, (void*)isrHandler251, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(252, (void*)isrHandler252, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(253, (void*)isrHandler253, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(254, (void*)isrHandler254, IDT_GATE_TYPE_INTERRUPT);
        registerHandler(255, (void*)isrHandler255, IDT_GATE_TYPE_INTERRUPT);
    }
};