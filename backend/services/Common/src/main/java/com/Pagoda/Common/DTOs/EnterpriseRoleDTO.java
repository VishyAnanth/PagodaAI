package com.Pagoda.Common.DTOs;

import java.util.List;

import com.Pagoda.Common.Enums.Permission;

import lombok.*;
import lombok.experimental.Accessors;

@Getter
@Setter
@AllArgsConstructor
@NoArgsConstructor
@Accessors(chain = true)
public class EnterpriseRoleDTO {
    private Long enterpriseId;
    private int roleId;
    private String description;
    private List<Permission> permissions;
}
