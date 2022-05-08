package com.Pagoda.Authorization;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.List;
import java.util.Set;
import java.util.stream.Collectors;
import java.util.stream.Stream;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import com.Pagoda.Authorization.DetailsServices.UserDetailsAuthImplementation;
import com.Pagoda.Authorization.DetailsServices.UserDetailsAuthService;
import com.Pagoda.Common.DTOs.*;
import com.Pagoda.Common.Enums.*;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.security.core.context.SecurityContextHolder;
import org.springframework.stereotype.Service;

@Service
public class UserService {

    @Autowired
    private UserDetailsAuthService userDetailsService;

    public AuthenticationOutputDTO getUserDetails(String username, HttpServletRequest request, HttpServletResponse response) {
        AuthenticationOutputDTO res = new AuthenticationOutputDTO()
        .setStatus(Status.SUCCESS);
        UserDetailsAuthImplementation details = (UserDetailsAuthImplementation)userDetailsService.loadUserByUsername(SecurityContextHolder.getContext().getAuthentication().getPrincipal().toString());
        List<UserRoleDTO> authorities = details.getAuthorities().stream().map(item -> item.getAuthority())
            .map(role -> new UserRoleDTO().setId(Long.valueOf(-1)).setUserId(Long.valueOf(-1))
            .setEnterpriseId(Long.parseLong(role.split(",")[0])).setRoleId(Integer.parseInt(role.split(",")[1]))).collect(Collectors.toList());
        UserDetailsAuthImplementation otherDetails = (UserDetailsAuthImplementation)userDetailsService.loadUserByUsername(username);
        if(otherDetails.getUser() == null) {
            return res.setStatus(Status.ERROR).setMessage("User not found");
        }
        List<UserRoleDTO> otherAuthorities = otherDetails.getAuthorities().stream().map(item -> item.getAuthority())
            .map(role -> new UserRoleDTO().setId(Long.valueOf(-1)).setUserId(Long.valueOf(-1))
            .setEnterpriseId(Long.parseLong(role.split(",")[0])).setRoleId(Integer.parseInt(role.split(",")[1])))
            .filter(authorities::contains).collect(Collectors.toList());
        if(otherAuthorities.size() == 0) {
            return res.setStatus(Status.ERROR).setMessage("Not permitted");
        }
        return res.setUser(otherDetails.getUser().unsetPassword());
    }

    public List<AuthenticationOutputDTO> getUsersDetails(List<String> usernames, HttpServletRequest request, HttpServletResponse response) {
        List<AuthenticationOutputDTO> res = new ArrayList<>();
        UserDetailsAuthImplementation details = (UserDetailsAuthImplementation)userDetailsService.loadUserByUsername(SecurityContextHolder.getContext().getAuthentication().getPrincipal().toString());
        List<UserRoleDTO> authorities = details.getAuthorities().stream().map(item -> item.getAuthority())
            .map(role -> new UserRoleDTO().setId(Long.valueOf(-1)).setUserId(Long.valueOf(-1))
            .setEnterpriseId(Long.parseLong(role.split(",")[0])).setRoleId(Integer.parseInt(role.split(",")[1]))).collect(Collectors.toList());
        List<UserDetailsAuthImplementation> userDetails = (List<UserDetailsAuthImplementation>)(List<?>)userDetailsService.loadUsersByUsername(usernames);
        userDetails.removeAll(Collections.singleton(null));
        for(UserDetailsAuthImplementation u : userDetails) {
            List<UserRoleDTO> userCommonAuthorities = u.getAuthorities().stream().map(item -> item.getAuthority())
            .map(role -> new UserRoleDTO().setId(Long.valueOf(-1)).setUserId(Long.valueOf(-1))
            .setEnterpriseId(Long.parseLong(role.split(",")[0])).setRoleId(Integer.parseInt(role.split(",")[1])))
            .filter(authorities::contains).collect(Collectors.toList());
            if(userCommonAuthorities.size() == 0) {
                res.add(new AuthenticationOutputDTO().setStatus(Status.ERROR));
                return res;
            }
        }
        res = userDetails.stream().map(u -> new AuthenticationOutputDTO().setUser(u.getUser().unsetPassword()).setStatus(Status.SUCCESS))
            .collect(Collectors.toList());
        return res;
    }

    public AuthenticationOutputDTO setUserDetails(AuthenticationInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        AuthenticationOutputDTO res = new AuthenticationOutputDTO()
        .setStatus(Status.SUCCESS);
        UserDetailsAuthImplementation details = (UserDetailsAuthImplementation)userDetailsService.loadUserByUsername(SecurityContextHolder.getContext().getAuthentication().getPrincipal().toString());
        UserDetailsAuthImplementation otherDetails = (UserDetailsAuthImplementation)userDetailsService.loadUserByUsername(dto.getUser().getUsername());
        if(otherDetails.getUser() == null || !details.getPassword().equals(otherDetails.getPassword()) || 
            !details.getUsername().equals(otherDetails.getUsername()) ||
            details.isAccountNonExpired() != otherDetails.isAccountNonExpired() ||
            details.isAccountNonLocked() != otherDetails.isAccountNonLocked() ||
            details.isCredentialsNonExpired() != otherDetails.isCredentialsNonExpired() ||
            details.isEnabled() != otherDetails.isEnabled() ||
            details.getUser().getVerified() != otherDetails.getUser().getVerified()) {
            return res.setStatus(Status.ERROR).setMessage("Not permitted");
        }
        details = (UserDetailsAuthImplementation)userDetailsService.editUser(dto.getUser());
        return res.setUser(details.getUser().unsetPassword().unsetId());
    }

    public Set<AuthenticationOutputDTO> searchForUser(List<AuthenticationInputDTO> dtos, HttpServletRequest request, HttpServletResponse response) {
        List<String> usernames = dtos.stream().map(dto -> dto.getUser().getUsername()).filter(s -> !s.isEmpty()).collect(Collectors.toList());
        List<String> firstNames = dtos.stream().map(dto -> dto.getUser().getUsername()).filter(s -> !s.isEmpty()).collect(Collectors.toList());
        List<String> lastNames = dtos.stream().map(dto -> dto.getUser().getUsername()).filter(s -> !s.isEmpty()).collect(Collectors.toList());
        List<String> emails = dtos.stream().map(dto -> dto.getUser().getUsername()).filter(s -> !s.isEmpty()).collect(Collectors.toList());

        List<UserDetailsAuthImplementation> usernameResults = userDetailsService.searchUserByUsernames(usernames).stream().map(elem -> (UserDetailsAuthImplementation)elem).collect(Collectors.toList());
        List<UserDetailsAuthImplementation> firstNameResults = userDetailsService.searchUserByUsernames(firstNames).stream().map(elem -> (UserDetailsAuthImplementation)elem).collect(Collectors.toList());
        List<UserDetailsAuthImplementation> lastNameResults = userDetailsService.searchUserByUsernames(lastNames).stream().map(elem -> (UserDetailsAuthImplementation)elem).collect(Collectors.toList());
        List<UserDetailsAuthImplementation> emailResults = userDetailsService.searchUserByUsernames(emails).stream().map(elem -> (UserDetailsAuthImplementation)elem).collect(Collectors.toList());

        Set<UserDetailsAuthImplementation> resultSet = Stream.of(usernameResults, firstNameResults, lastNameResults, emailResults).flatMap(Collection::stream).collect(Collectors.toSet());
        Set<AuthenticationOutputDTO> resultAuthenticationOutputDTOs = resultSet.stream().map(elem -> new AuthenticationOutputDTO().setStatus(Status.SUCCESS).setUser(elem.getUser().unsetPassword())).collect(Collectors.toSet());
        return resultAuthenticationOutputDTOs;
    }

    public List<AuthenticationOutputDTO> getEnterprisesForUser(AuthenticationInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        List<AuthenticationOutputDTO> resList = new ArrayList<>();
        UserDetailsAuthImplementation details = (UserDetailsAuthImplementation)userDetailsService.loadUserByUsername(SecurityContextHolder.getContext().getAuthentication().getPrincipal().toString());
        List<UserRoleDTO> authorities = details.getAuthorities().stream().map(item -> item.getAuthority())
            .map(role -> new UserRoleDTO().setId(Long.valueOf(-1)).setUserId(Long.valueOf(-1))
            .setEnterpriseId(Long.parseLong(role.split(",")[0])).setRoleId(Integer.parseInt(role.split(",")[1]))).collect(Collectors.toList());
        UserDetailsAuthImplementation otherDetails = (UserDetailsAuthImplementation)userDetailsService.loadUserByUsername(dto.getUser().getUsername());
        if(otherDetails.getUser() == null) {
            resList.add(new AuthenticationOutputDTO().setStatus(Status.ERROR).setMessage("User not found"));
            return resList;
        }
        List<UserRoleDTO> otherAuthorities = otherDetails.getAuthorities().stream().map(item -> item.getAuthority())
            .map(role -> new UserRoleDTO().setId(Long.valueOf(-1)).setUserId(Long.valueOf(-1))
            .setEnterpriseId(Long.parseLong(role.split(",")[0])).setRoleId(Integer.parseInt(role.split(",")[1])))
            .filter(authorities::contains).collect(Collectors.toList());
        if(otherAuthorities.size() == 0) {
            resList.add(new AuthenticationOutputDTO().setStatus(Status.ERROR).setMessage("Not permitted"));
            return resList;
        }
        List<EnterpriseDTO> enterprises = userDetailsService.loadEnterprisesByUserUsername(dto.getUser().getUsername());
        resList = enterprises.stream().map(enterprise -> new AuthenticationOutputDTO().setStatus(Status.SUCCESS).setEnterprise(enterprise.unsetId()))
        .collect(Collectors.toList());
        return resList;
    }
}
