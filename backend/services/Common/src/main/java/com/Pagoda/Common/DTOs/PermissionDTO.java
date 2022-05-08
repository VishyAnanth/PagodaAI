package com.Pagoda.Common.DTOs;

import com.Pagoda.Common.Enums.Permission;

import lombok.*;
import lombok.experimental.Accessors;

@Getter
@Setter
@AllArgsConstructor
@NoArgsConstructor
@Accessors(chain = true)
public class PermissionDTO {
    private Permission permission;
    private String description;
}

