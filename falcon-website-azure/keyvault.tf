data "azurerm_client_config" "current" {}

data "azurerm_subscription" "primary" {}

data "azurerm_user_assigned_identity" "stephen" {
	name = "${azurerm_kubernetes_cluster.default.name}-agentpool"
	resource_group_name = azurerm_kubernetes_cluster.default.node_resource_group
}

resource "azurerm_user_assigned_identity" "aks_pod_identity" {
	resource_group_name = azurerm_resource_group.default.name
	location            = azurerm_resource_group.default.location
	name                = "falcon-website-pod-identity"
}

resource "azurerm_role_assignment" "aks_identity_operator" {
	scope                = azurerm_user_assigned_identity.aks_pod_identity.id
	role_definition_name = "Managed Identity Operator"
	principal_id         = azurerm_kubernetes_cluster.default.kubelet_identity[0].object_id
}

resource "azurerm_key_vault" "falcon-website-keyvault" {
	name = "falcon-website-keyvault"
	location = azurerm_resource_group.default.location
	enabled_for_deployment = true
	resource_group_name = azurerm_resource_group.default.name
	tenant_id = data.azurerm_client_config.current.tenant_id
	sku_name = "standard"
	access_policy {
		tenant_id = data.azurerm_client_config.current.tenant_id
		object_id = data.azurerm_client_config.current.object_id

		certificate_permissions = [
			"backup", "create", "delete", "deleteissuers", "get", "getissuers", "import",
			"list", "listissuers", "managecontacts", "manageissuers", "purge", "recover",
			"restore", "setissuers", "update"
		]

		key_permissions = [
			"get", "list", "create", "delete", 
		]

		secret_permissions = [
			"backup", "delete", "get", "list", "purge", "recover", "restore", "set"
		]

		storage_permissions = [
			"get", "list"
		]
	}

	access_policy {
		tenant_id = data.azurerm_client_config.current.tenant_id
		object_id = data.azurerm_user_assigned_identity.stephen.principal_id

		certificate_permissions = [ "get", "list" ]
		key_permissions = [ "get", "list" ]
		secret_permissions = [ "get", "list" ]
		storage_permissions = [ "get", "list" ]
	}

	network_acls {
		default_action = "Deny"
		bypass         = "AzureServices"
		ip_rules = [ "0.0.0.0/0" ]
	}
	tags = {
		environment = "Demo"
	}
}

resource "azurerm_role_assignment" "contributor_role" {
	scope                = data.azurerm_subscription.primary.id
	role_definition_name             = "Contributor"
	principal_id                     = data.azurerm_user_assigned_identity.stephen.principal_id
	skip_service_principal_aad_check = true
}

resource "azurerm_key_vault_secret" "example" {
	name = "falcon-website-cert"
	value = "certificate_file_data_here"
	key_vault_id = azurerm_key_vault.falcon-website-keyvault.id

	tags = {
		environment = "Demo"
	}
}

resource "azurerm_key_vault_certificate" "bengreen-eu-ssl" {
	name         = "bengreen-eu-ssl"
	key_vault_id = azurerm_key_vault.falcon-website-keyvault.id

	certificate {
		contents = filebase64("bengreen.eu.pfx")
		password = ""
	}

	certificate_policy {
		issuer_parameters {
			name = "Self"
		}

		key_properties {
			exportable = true
			key_size = 4096
			key_type = "RSA"
			reuse_key = false
		}

		secret_properties {
			content_type = "application/x-pkcs12"
		}
	}
}

