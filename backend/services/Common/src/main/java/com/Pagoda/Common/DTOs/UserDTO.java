package com.Pagoda.Common.DTOs;

import java.util.Date;

import lombok.*;
import lombok.experimental.Accessors;

@Getter
@Setter
@NoArgsConstructor
@AllArgsConstructor
@Accessors(chain = true)
public class UserDTO {
    private Long id;
    private String username;
    private String password;
    private String email;
    private String profilePic;
    private String firstName;
    private String lastName;
    private String bio;
    private String website;
    private String githubUsername;
    private String token;
    private Date tokenExpiration;

    private Boolean nonExpired;
    private Boolean nonLocked;
    private Boolean credentialsNonExpired;
    private Boolean enabled;
    private Boolean verified;

    public UserDTO unsetPassword() {
        this.password = "";
        return this;
    }
    
    public UserDTO unsetEmail() {
        this.email = "";
        return this;
    }

    public UserDTO unsetId() {
        this.id = Long.valueOf(-1);
        return this;
    }
}