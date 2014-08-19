#!/bin/sh

rmmod kvm_intel
rmmod kvm 
insmod x86/kvm.ko
insmod x86/kvm-intel.ko
