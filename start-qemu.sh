qemu-system-x86_64 \
	-drive file=zso2016.img,if=virtio \
	-enable-kvm \
	-smp 3 \
	-net nic,model=virtio -net user \
	-m 1G -balloon virtio \
	-fsdev local,id=hshare,path=hshare/,security_model=none -device virtio-9p-pci,fsdev=hshare,mount_tag=hshare \
	-chardev stdio,id=cons,signal=off -device virtio-serial-pci -device virtconsole,chardev=cons \
	-display none
