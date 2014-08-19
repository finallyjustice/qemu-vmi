#./configure --prefix=/test/qemu --target-list=i386-softmmu,x86_64-softmmu
#/test/qemu/bin/qemu-system-x86_64 -hda /media/Disk/LinuxSystem/12ubuntu/ubuntu12 -m 1024m -monitor stdio -enable-kvm
/test/qemu/bin/qemu-system-x86_64 -smp 4 -hda /media/Disk/LinuxSystem/ubuntu12.04.4/ubuntu120404.qcow -monitor stdio -m 1024m -enable-kvm
