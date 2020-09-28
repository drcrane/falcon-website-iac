provider "libvirt" {
	uri = "qemu:///system"
}

variable "hostname" { default = "debian" }
variable "domain" { default = "localdomain" }

# Define a special pool (not used because this system does not use btrfs)
#resource "libvirt_pool" "debian" {
#	name = "debian"
#	type = "dir"
#	path = "/tmp/libvirt_pool_debian"
#}

# Using a pre-defined "default" pool (see comment above)
resource "libvirt_volume" "base_os_image" {
	name = "base_os_image"
	pool = "default"
	# could also be an http or https URL.
	source = "/root/VMs/IMGs/debian-10.5.1-20200830-openstack-amd64.qcow2"
	format = "qcow2"
}

resource "libvirt_volume" "debian_qcow2" {
	name = "debian_qcow2"
	pool = "default"
	base_volume_id = libvirt_volume.base_os_image.id
	format = "qcow2"
	size = 17179869184
}

data "template_file" "user_data" {
	template = file("${path.module}/cloud_init.cfg")
	vars = {
		hostname = var.hostname
		fqdn = "${var.hostname}.${var.domain}"
	}
}

data "template_file" "network_config" {
	template = file("${path.module}/network_config.cfg")
}

resource "libvirt_cloudinit_disk" "commoninit" {
	name = "commoninit.iso"
	user_data = data.template_file.user_data.rendered
	network_config = data.template_file.network_config.rendered
	pool = "default"
}

resource "libvirt_domain" "domain_debian" {
	name = "debian-terraform"
	memory = "2048"
	vcpu = 2

	cloudinit = libvirt_cloudinit_disk.commoninit.id
	
	disk {
		volume_id = libvirt_volume.debian_qcow2.id
	}

	network_interface {
		network_name = "default"
		mac = "52:54:00:EA:96:DA"
		wait_for_lease = true
	}

	# this is required for the openstack base image or the VM will
	# stop responding early in the boot process.
	console {
		type = "pty"
		target_port = "0"
		target_type = "serial"
	}

	console {
		type = "pty"
		target_type = "virtio"
		target_port = "1"
	}

	video {
		type = "vga"
	}

	graphics {
		type = "vnc"
		listen_type = "address"
		autoport = true
	}
}

output "ips" {
	value = libvirt_domain.domain_debian.*.network_interface.0.addresses
}

terraform {
	required_version = ">= 0.13"
	required_providers {
		libvirt = {
			versions = ["0.0.1"]
			source = "bengreen.eu/corp/libvirt"
		}
	}
}

