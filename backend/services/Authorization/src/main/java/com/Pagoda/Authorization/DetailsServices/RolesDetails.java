package com.Pagoda.Authorization.DetailsServices;

import com.Pagoda.Common.DTOs.EnterpriseRoleDTO;
import com.Pagoda.Common.DTOs.UserRoleDTO;

public class RolesDetails {
    private UserRoleDTO role;
    private EnterpriseRoleDTO enterpriseRole;

    public RolesDetails(UserRoleDTO role, EnterpriseRoleDTO enterpriseRole) {
        this.role = role;
        this.enterpriseRole = enterpriseRole;
    }

    public static RolesDetails build(UserRoleDTO role, EnterpriseRoleDTO enterpriseRole) {
        return new RolesDetails(role, enterpriseRole);
    }

    public UserRoleDTO getRole() {
        return this.role;
    }

    public EnterpriseRoleDTO getEnterpriseRole() {
        return this.enterpriseRole;
    }
}
