package com.Pagoda.Authorization.DetailsServices;

import java.util.List;

import com.Pagoda.Common.DTOs.EnterpriseDTO;
import com.Pagoda.Common.DTOs.EnterpriseRoleDTO;
import com.Pagoda.Common.DTOs.UserDTO;
import com.Pagoda.Common.DTOs.UserRoleDTO;

import org.springframework.stereotype.Service;

@Service
public class RolesService {
    
    public RolesDetails createRole(UserRoleDTO role) {
        return null;
    }

    public void deleteRole(UserRoleDTO role) {
        
    }

    public void deleteRoleByUserId(Long userId) {
        
    }

    public RolesDetails createEnterpriseRole(EnterpriseRoleDTO enterpriseRole) {
        return null;
    }

    public RolesDetails editEnterpriseRole(EnterpriseRoleDTO enterpriseRole) {
        return null;
    }

    public void deleteEnterpriseRole(EnterpriseRoleDTO enterpriseRole) {
        
    }

    public RolesDetails getRole(UserRoleDTO role) {
        return null;
    }

    public List<RolesDetails> getRoles(List<UserRoleDTO> roles) {
        return null;
    }

    public RolesDetails getEnterpriseRole(EnterpriseRoleDTO enterpriseRole) {
        return null;
    }

    public RolesDetails getEnterpriseRoleByEnterpriseAndRoleId(Long enterpriseId, int roleId) {
        return null;
    }

    public RolesDetails createAdminRoleForUserAndEnterprise(UserDTO user, EnterpriseDTO enterprise) {
        
        return null;
    }

}
