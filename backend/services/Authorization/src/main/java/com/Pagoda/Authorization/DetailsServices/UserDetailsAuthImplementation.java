package com.Pagoda.Authorization.DetailsServices;

import org.springframework.security.core.GrantedAuthority;
import org.springframework.security.core.authority.SimpleGrantedAuthority;
import org.springframework.security.core.userdetails.UserDetails;
import java.util.stream.Collectors;
import java.util.Collection;
import java.util.List;
import java.util.Objects;

import com.Pagoda.Common.DTOs.*;

public class UserDetailsAuthImplementation implements UserDetails {
    private UserDTO userDto;
    private List<UserRoleDTO> roles;

    private Collection<? extends GrantedAuthority> authorities;

    public UserDetailsAuthImplementation(UserDTO userDto, List<UserRoleDTO> roles, Collection<? extends GrantedAuthority> authorities) {
        this.userDto = userDto;
        this.authorities = authorities;
        this.roles = roles;
    }

    public static UserDetailsAuthImplementation build(UserDTO user, List<UserRoleDTO> roles) {
        List<GrantedAuthority> authorities = roles.stream().map(role -> new SimpleGrantedAuthority(String.valueOf(role.getEnterpriseId()) + "," + String.valueOf(role.getRoleId())))
        .collect(Collectors.toList());

        return new UserDetailsAuthImplementation(user, roles, authorities);
    }

    @Override
	public Collection<? extends GrantedAuthority> getAuthorities() {
		return authorities;
	}

    @Override
    public String getPassword() {
        return userDto.getPassword();
    }

    @Override
    public String getUsername() {
        return userDto.getUsername().toString();
    }

    @Override
	public boolean isAccountNonExpired() {
		return userDto.getNonExpired();
	}

	@Override
	public boolean isAccountNonLocked() {
		return userDto.getNonLocked();
	}

	@Override
	public boolean isCredentialsNonExpired() {
		return userDto.getCredentialsNonExpired();
	}

	@Override
	public boolean isEnabled() {
		return userDto.getEnabled();
	}

    @Override
    public boolean equals(Object o) {
        if(this == o)
            return true;
        if(o == null || getClass() != o.getClass())
            return false;
        UserDetailsAuthImplementation other = (UserDetailsAuthImplementation) o;
        return Objects.equals(userDto.getId(), other.userDto.getId());
    }

    public UserDTO getUser() {
        return userDto;
    }

    public List<UserRoleDTO> getRoles() {
        return this.roles;
    }
}