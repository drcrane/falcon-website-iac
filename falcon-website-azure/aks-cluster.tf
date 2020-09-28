provider "azurerm" {
	version = "~> 2.0"
	features {}
}

resource "azurerm_resource_group" "default" {
	name = "${var.application_name}-rg"
	location = "UK West"

	tags = {
		environment = "Demo"
	}
}

resource "azurerm_kubernetes_cluster" "default" {
	name = "${var.application_name}-aks"
	location = azurerm_resource_group.default.location
	resource_group_name = azurerm_resource_group.default.name
	dns_prefix = "${var.application_name}-k8s"

	default_node_pool {
		name            = "default"
		node_count      = 2
		vm_size         = "Standard_D2_v2"
		os_disk_size_gb = 30
	}

	identity {
		type = "SystemAssigned"
	}

#	service_principal {
#		client_id     = var.client_id
#		client_secret = var.client_secret
#	}

	role_based_access_control {
		enabled = true
	}

	tags = {
		environment = "Demo"
	}
}
