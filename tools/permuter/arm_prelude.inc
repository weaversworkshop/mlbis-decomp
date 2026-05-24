.syntax unified
.cpu arm946e-s

.macro glabel label, visibility=global
    .\visibility \label
    .type \label, %function
    \label:
.endm

.macro dlabel label, visibility=global
    .\visibility \label
    .type \label, %object
    \label:
.endm

.macro jlabel label, visibility=local
    \label:
.endm

.section .text
