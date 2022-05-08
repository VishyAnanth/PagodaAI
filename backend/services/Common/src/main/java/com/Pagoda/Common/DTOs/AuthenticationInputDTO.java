package com.Pagoda.Common.DTOs;

import lombok.*;
import lombok.experimental.Accessors;


@Getter
@Setter
@NoArgsConstructor
@AllArgsConstructor
@Accessors(chain = true)
public class AuthenticationInputDTO {
    private UserDTO user;
    private UserRoleDTO role;
    private EnterpriseDTO enterprise;
    private EnterpriseRoleDTO enterpriseRole;
}