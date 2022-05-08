package com.Pagoda.Authorization.DetailsServices;

import com.Pagoda.Common.DTOs.*;

public class EnterpriseDetails {
    private EnterpriseDTO enterprise;

    public EnterpriseDetails(EnterpriseDTO enterprise) {
        this.enterprise = enterprise;
    }

    public static EnterpriseDetails build(EnterpriseDTO enterprise) {
        return new EnterpriseDetails(enterprise);
    }

    public EnterpriseDTO getEnterprise() {
        return this.enterprise;
    }
}
