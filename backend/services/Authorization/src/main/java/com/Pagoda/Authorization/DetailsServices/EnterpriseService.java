package com.Pagoda.Authorization.DetailsServices;

import java.util.List;

import com.Pagoda.Common.DTOs.EnterpriseDTO;
import com.Pagoda.Common.DTOs.UserDTO;

import org.springframework.stereotype.Service;

@Service
public class EnterpriseService {
    
    public EnterpriseDetails loadEnterprisebyName(String name) {
        //pull this from database
        return null;
    }

    public EnterpriseDetails createEnterprise(EnterpriseDTO enterprise) {
        //fill this in
        return EnterpriseDetails.build(enterprise);
    }

    public List<UserDTO> loadUsersByEnterprise(String name) {
        return null;
    }
}
