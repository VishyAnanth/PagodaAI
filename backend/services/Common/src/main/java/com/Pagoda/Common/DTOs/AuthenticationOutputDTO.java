package com.Pagoda.Common.DTOs;

import lombok.*;
import lombok.experimental.Accessors;

import java.util.Set;

import com.Pagoda.Common.Enums.*;

@Getter
@Setter
@NoArgsConstructor
@AllArgsConstructor
@Accessors(chain = true)
public class AuthenticationOutputDTO {
    private Status status;
    private String token;
    private UserDTO user;
    private EnterpriseDTO enterprise;
    private String message;
    private Set<Permission> permissions;
}