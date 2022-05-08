package com.Pagoda.Common.DTOs;

import lombok.*;
import lombok.experimental.Accessors;

@Getter
@Setter
@NoArgsConstructor
@AllArgsConstructor
@Accessors(chain = true)
public class EnterpriseDTO {
    private Long id;
    private String name;
    private String logo;
    private String website;

    private Boolean nonExpired;
    private Boolean nonLocked;
    private Boolean credentialsNonExpired;
    private Boolean enabled;
    private Boolean verified;

    public EnterpriseDTO unsetId() {
        this.id = Long.valueOf(-1);
        return this;
    }
}
