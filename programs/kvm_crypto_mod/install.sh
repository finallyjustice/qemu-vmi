# script to install driver in guest VM
rmmod kvm-crypto
rm /dev/kvm-crypto
insmod kvm-crypto.ko
mknod /dev/kvm-crypto c 50 0
chmod 666 /dev/kvm-crypto
