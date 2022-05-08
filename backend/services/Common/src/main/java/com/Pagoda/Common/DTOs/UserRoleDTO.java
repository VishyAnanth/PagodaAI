package com.Pagoda.Common.DTOs;

import lombok.*;
import lombok.experimental.Accessors;

@Getter
@Setter
@NoArgsConstructor
@AllArgsConstructor
@Accessors(chain = true)
public class UserRoleDTO {
    private Long id;
    private Long userId;
    private Long enterpriseId;
    private int roleId;

    public boolean equals(Object obj) {
        if(obj == this) {
            return true;
        }

        if(obj == null || obj.getClass() != this.getClass()) {
            return false;
        }

        UserRoleDTO other = (UserRoleDTO)obj;
        return other.getRoleId() == this.getRoleId() && other.getEnterpriseId() == this.getEnterpriseId();
    }
}