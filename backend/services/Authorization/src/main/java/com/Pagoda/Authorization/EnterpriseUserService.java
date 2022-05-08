package com.Pagoda.Authorization;

import java.security.SecureRandom;
import java.util.Date;
import java.util.List;
import java.util.Set;
import java.util.stream.Collectors;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import com.Pagoda.Authorization.DetailsServices.EnterpriseDetails;
import com.Pagoda.Authorization.DetailsServices.EnterpriseService;
import com.Pagoda.Authorization.DetailsServices.RolesDetails;
import com.Pagoda.Authorization.DetailsServices.RolesService;
import com.Pagoda.Authorization.DetailsServices.UserDetailsAuthImplementation;
import com.Pagoda.Authorization.DetailsServices.UserDetailsAuthService;
import com.Pagoda.Authorization.FeignClients.EmailFeignClient;
import com.Pagoda.Authorization.Security.JwtUtils;
import com.Pagoda.Common.DTOs.*;
import com.Pagoda.Common.Enums.*;
import com.netflix.discovery.shared.Pair;

import org.apache.commons.validator.routines.EmailValidator;
import org.apache.commons.validator.routines.UrlValidator;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.security.authentication.AuthenticationManager;
import org.springframework.security.authentication.UsernamePasswordAuthenticationToken;
import org.springframework.security.core.Authentication;
import org.springframework.security.core.context.SecurityContextHolder;
import org.springframework.security.crypto.argon2.Argon2PasswordEncoder;
import org.springframework.security.crypto.password.PasswordEncoder;
import org.springframework.security.web.authentication.logout.SecurityContextLogoutHandler;
import org.springframework.stereotype.Service;

@Service
public class EnterpriseUserService {
    
    @Autowired
    private AuthenticationManager authenticationManager;

    @Autowired
    private EnterpriseService enterpriseService;

    @Autowired
    private UserDetailsAuthService userDetailsService;

    @Autowired
    private RolesService rolesService;

    @Autowired
    private EmailFeignClient emailFeignClient;

    @Autowired
	private JwtUtils jwtUtils;

    public PasswordEncoder passwordEncoder() {
		return new Argon2PasswordEncoder(16, 32, 4, 1048576, 2);
	}

    private Pair<AuthenticationOutputDTO, UserDetailsAuthImplementation> validAuth() {
        AuthenticationOutputDTO res = new AuthenticationOutputDTO()
        .setStatus(Status.SUCCESS);
        Authentication auth = SecurityContextHolder.getContext().getAuthentication();
        if(auth == null) {
            return new Pair<>(res.setStatus(Status.ERROR).setMessage("Invalid JWT"), null);
        }
        UserDetailsAuthImplementation details = (UserDetailsAuthImplementation)userDetailsService.loadUserByUsername(SecurityContextHolder.getContext().getAuthentication().getPrincipal().toString().split(",")[0]);
        if(!details.getUser().getEnabled()) {
            return new Pair<>(res.setStatus(Status.ERROR).setMessage("Unverified account"),null);
        }
        String token = SecurityContextHolder.getContext().getAuthentication().getDetails().toString();
        if(!details.getUser().getToken().equals(token)) {
            return new Pair<>(res.setStatus(Status.ERROR).setMessage("Invalid JWT"),null);
        }
        return new Pair<>(res, details);
    }

    public AuthenticationOutputDTO createEnterprise(AuthenticationInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        Pair<AuthenticationOutputDTO, UserDetailsAuthImplementation> temp = validAuth();
        AuthenticationOutputDTO res = temp.first();
        UserDetailsAuthImplementation details = temp.second();
        if(res.getStatus().equals(Status.ERROR)) {
            return res;
        }
        if(!UrlValidator.getInstance().isValid(dto.getEnterprise().getWebsite()) || dto.getEnterprise().getName().length() == 0 ||
        dto.getEnterprise().getLogo().length() == 0) {
            return res.setStatus(Status.ERROR).setMessage("Invalid enterprise details");
        }

        List<UserRoleDTO> roles = details.getRoles();
        Set<Permission> permissions = rolesService.getRoles(roles).stream().map(elem -> elem.getEnterpriseRole().getPermissions()).flatMap(List::stream).collect(Collectors.toSet());
        if(!permissions.contains(Permission.WRITE_ENTERPRISES)) {
            return res.setStatus(Status.ERROR).setMessage("Invalid permissions");
        }
        if((EnterpriseDetails)enterpriseService.loadEnterprisebyName(dto.getEnterprise().getName()) != null) {
            return res.setStatus(Status.ERROR).setMessage("Enterprise name already taken");
        }
        
        dto.getEnterprise().setId(new SecureRandom().nextLong());
        enterpriseService.createEnterprise(dto.getEnterprise());
        rolesService.createAdminRoleForUserAndEnterprise(details.getUser(), dto.getEnterprise());
        return res.setEnterprise(dto.getEnterprise().unsetId());
    }

    public AuthenticationOutputDTO signInToEnterprise(AuthenticationInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        AuthenticationOutputDTO res = new AuthenticationOutputDTO()
        .setStatus(Status.SUCCESS);
        Authentication auth = SecurityContextHolder.getContext().getAuthentication();
        if(auth != null) {
            new SecurityContextLogoutHandler().logout(request, response, SecurityContextHolder.getContext().getAuthentication());
        }
        Authentication authentication = authenticationManager.authenticate(new UsernamePasswordAuthenticationToken(dto.getUser().getUsername(), dto.getUser().getPassword()));
        SecurityContextHolder.getContext().setAuthentication(authentication);
        UserDetailsAuthImplementation details = (UserDetailsAuthImplementation) authentication.getPrincipal();
        if(!details.getUser().getEnabled()) {
            return res.setStatus(Status.ERROR).setMessage("Unverified account");
        }
        EnterpriseDetails enterpriseDetails = (EnterpriseDetails)enterpriseService.loadEnterprisebyName(dto.getEnterprise().getName());
        List<UserRoleDTO> roles = details.getRoles().stream().filter(elem -> elem.getEnterpriseId().equals(enterpriseDetails.getEnterprise().getId())).collect(Collectors.toList());
        Set<Permission> permissions = rolesService.getRoles(roles).stream().map(elem -> elem.getEnterpriseRole().getPermissions()).flatMap(List::stream).collect(Collectors.toSet());
        if(!permissions.contains(Permission.READ_ENTERPRISES)) {
            return res.setStatus(Status.ERROR).setMessage("Invalid permissions");
        }
        
        String jwt = jwtUtils.generateEnterpriseToken(authentication, enterpriseDetails.getEnterprise().getName());
        userDetailsService.editUser(details.getUser().setToken(jwt));
        res.setEnterprise(enterpriseDetails.getEnterprise().unsetId()).setUser(details.getUser().unsetPassword().unsetId())
        .setToken(jwt);

        return res;
    }

    // public AuthenticationOutputDTO signInToEnterpriseSAML(AuthenticationInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
    //     AuthenticationOutputDTO res = new AuthenticationOutputDTO()
    //     .setStatus(Status.SUCCESS);
    //     //fill this in
    //     return res;
    // }

    public AuthenticationOutputDTO signOutOfEnterprise(AuthenticationInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        AuthenticationOutputDTO res = new AuthenticationOutputDTO()
        .setStatus(Status.SUCCESS);
        Authentication auth = SecurityContextHolder.getContext().getAuthentication();
        if(auth == null) {
            return res.setStatus(Status.ERROR).setMessage("Invalid JWT");
        }
        new SecurityContextLogoutHandler().logout(request, response, SecurityContextHolder.getContext().getAuthentication());
        Authentication authentication = authenticationManager.authenticate(new UsernamePasswordAuthenticationToken(dto.getUser().getUsername(), dto.getUser().getPassword()));
        SecurityContextHolder.getContext().setAuthentication(authentication);
        String jwt = jwtUtils.generateToken(authentication);
        UserDetailsAuthImplementation userDetails = (UserDetailsAuthImplementation) authentication.getPrincipal();
        userDetailsService.editUser(userDetails.getUser().setToken(jwt));
        res.setUser(userDetails.getUser().unsetPassword().unsetId()).setToken(jwt);
        return res;
    }

    public AuthenticationOutputDTO addUserToEnterprise(AuthenticationInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        Pair<AuthenticationOutputDTO, UserDetailsAuthImplementation> temp = validAuth();
        AuthenticationOutputDTO res = temp.first();
        UserDetailsAuthImplementation details = temp.second();
        EnterpriseDetails enterpriseDetails = (EnterpriseDetails)enterpriseService.loadEnterprisebyName(SecurityContextHolder.getContext().getAuthentication().getPrincipal().toString().split((","))[1]);
        List<UserRoleDTO> roles = details.getRoles().stream().filter(elem -> elem.getEnterpriseId().equals(enterpriseDetails.getEnterprise().getId())).collect(Collectors.toList());
        Set<Permission> permissions = rolesService.getRoles(roles).stream().map(elem -> elem.getEnterpriseRole().getPermissions()).flatMap(List::stream).collect(Collectors.toSet());
        if(!permissions.contains(Permission.WRITE_ENTERPRISE_USERS)) {
            return res.setStatus(Status.ERROR).setMessage("Invalid permissions");
        }

        UserDetailsAuthImplementation subject = (UserDetailsAuthImplementation)userDetailsService.loadUserByUsername(dto.getUser().getUsername());
        if(subject == null) {
            return res.setStatus(Status.ERROR).setMessage("Invalid subject user");
        }
        if(!subject.getUser().getEnabled()) {
            return res.setStatus(Status.ERROR).setMessage("Unverified subject account");
        }
        RolesDetails rolesDetails = rolesService.getEnterpriseRoleByEnterpriseAndRoleId(enterpriseDetails.getEnterprise().getId(), dto.getRole().getRoleId());
        if(rolesDetails == null) {
            return res.setStatus(Status.ERROR).setMessage("Invalid role");
        }

        dto.getRole().setEnterpriseId(enterpriseDetails.getEnterprise().getId())
        .setUserId(subject.getUser().getId())
        .setId(new SecureRandom().nextLong());

        rolesService.createRole(dto.getRole());
        return res.setUser(dto.getUser().unsetPassword().unsetId()).setEnterprise(dto.getEnterprise());

    }

    public AuthenticationOutputDTO inviteExternalEmailToEnterprise(AuthenticationInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        Pair<AuthenticationOutputDTO, UserDetailsAuthImplementation> temp = validAuth();
        AuthenticationOutputDTO res = temp.first();
        UserDetailsAuthImplementation details = temp.second();
        String email = dto.getUser().getEmail();
        if(!EmailValidator.getInstance().isValid(email)) {
            return res.setStatus(Status.ERROR).setMessage("Invalid email address");
        }
        EnterpriseDetails enterpriseDetails = (EnterpriseDetails)enterpriseService.loadEnterprisebyName(SecurityContextHolder.getContext().getAuthentication().getPrincipal().toString().split((","))[1]);
        List<UserRoleDTO> roles = details.getRoles().stream().filter(elem -> elem.getEnterpriseId().equals(enterpriseDetails.getEnterprise().getId())).collect(Collectors.toList());
        Set<Permission> permissions = rolesService.getRoles(roles).stream().map(elem -> elem.getEnterpriseRole().getPermissions()).flatMap(List::stream).collect(Collectors.toSet());
        if(!permissions.contains(Permission.WRITE_ENTERPRISE_USERS)) {
            return res.setStatus(Status.ERROR).setMessage("Invalid permissions");
        }

        UserDetailsAuthImplementation subjectDetails = (UserDetailsAuthImplementation)userDetailsService.loadUserByEmail(email);
        if(subjectDetails != null && !subjectDetails.getUser().getUsername().equals("")) {
            dto.setUser(subjectDetails.getUser());
            return this.addUserToEnterprise(dto, request, response);
        }
        if(subjectDetails != null) {
            userDetailsService.editUser(subjectDetails.getUser()
            .setToken(String.valueOf(new SecureRandom().nextLong()))
            .setTokenExpiration(new Date(System.currentTimeMillis() + 86400000))
            .setPassword(passwordEncoder().encode(String.valueOf(new SecureRandom().nextLong())))
            .setUsername(String.valueOf(new SecureRandom().nextLong())));
                        
            emailFeignClient.sendEmail(new EmailInputDTO()
            .setRecipient(subjectDetails.getUser().getEmail()));//FIX THIS

            return res;
        }
        UserDTO newUser = new UserDTO().setId(new SecureRandom().nextLong())
        .setEmail(email)
        .setToken(String.valueOf(new SecureRandom().nextLong()))
        .setTokenExpiration(new Date(System.currentTimeMillis() + 86400000))
        .setPassword(passwordEncoder().encode(String.valueOf(new SecureRandom().nextLong())))
        .setUsername(String.valueOf(new SecureRandom().nextLong()));
        userDetailsService.createUser(newUser);
        
        rolesService.createRole(dto.getRole()
        .setId(new SecureRandom().nextLong())
        .setEnterpriseId(enterpriseDetails.getEnterprise().getId())
        .setUserId(newUser.getId()));
        
        emailFeignClient.sendEmail(new EmailInputDTO()
        .setRecipient(newUser.getEmail()));//FIX THIS

        return res;
    }

    public AuthenticationOutputDTO acceptInvitationExternal(AuthenticationInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        AuthenticationOutputDTO res = new AuthenticationOutputDTO()
        .setStatus(Status.SUCCESS);

        String email = dto.getUser().getEmail();
        if(!EmailValidator.getInstance().isValid(email)) {
            return res.setStatus(Status.ERROR).setMessage("Invalid email address");
        }

        UserDetailsAuthImplementation details = (UserDetailsAuthImplementation)userDetailsService.loadUserByEmail(email);
        if(details == null) {
            return res.setStatus(Status.ERROR).setMessage("Invalid email address");
        }
        if(!details.getUser().getToken().equals(dto.getUser().getToken())) {
            return res.setStatus(Status.ERROR).setMessage("Invalid token");
        }
        if(dto.getUser().getUsername().length() == 0 || dto.getUser().getPassword().length() == 0 ||
        dto.getUser().getEmail().length() == 0 || dto.getUser().getFirstName().length() == 0 ||
        dto.getUser().getLastName().length() == 0 || !EmailValidator.getInstance().isValid(dto.getUser().getEmail())) {
            return res.setStatus(Status.ERROR).setMessage("Invalid fields");
        }
        dto.getUser().setPassword(passwordEncoder().encode(dto.getUser().getPassword()));
        userDetailsService.editUserToUser(details.getUser(), dto.getUser());
        return res;
    }

    public AuthenticationOutputDTO declineInvitationExternal(AuthenticationInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        AuthenticationOutputDTO res = new AuthenticationOutputDTO()
        .setStatus(Status.SUCCESS);
        String email = dto.getUser().getEmail();
        if(!EmailValidator.getInstance().isValid(email)) {
            return res.setStatus(Status.ERROR).setMessage("Invalid email address");
        }
        UserDetailsAuthImplementation details = (UserDetailsAuthImplementation)userDetailsService.loadUserByEmail(email);
        if(details == null) {
            return res.setStatus(Status.ERROR).setMessage("Invalid email address");
        }
        if(!details.getUser().getToken().equals(dto.getUser().getToken())) {
            return res.setStatus(Status.ERROR).setMessage("Invalid token");
        }
        userDetailsService.deleteUser(details.getUser());
        rolesService.deleteRoleByUserId(details.getUser().getId());
        return res;
    }

    public AuthenticationOutputDTO addRoleToEnterprise(AuthenticationInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        Pair<AuthenticationOutputDTO, UserDetailsAuthImplementation> temp = validAuth();
        AuthenticationOutputDTO res = temp.first();
        UserDetailsAuthImplementation details = temp.second();
        if(dto.getEnterpriseRole().getDescription().length() == 0 || 
        dto.getEnterpriseRole().getPermissions().size() == 0) {
            return res.setStatus(Status.ERROR).setMessage("Invalid role parameters");
        }
        EnterpriseDetails enterpriseDetails = (EnterpriseDetails)enterpriseService.loadEnterprisebyName(SecurityContextHolder.getContext().getAuthentication().getPrincipal().toString().split((","))[1]);
        List<UserRoleDTO> roles = details.getRoles().stream().filter(elem -> elem.getEnterpriseId().equals(enterpriseDetails.getEnterprise().getId())).collect(Collectors.toList());
        Set<Permission> permissions = rolesService.getRoles(roles).stream().map(elem -> elem.getEnterpriseRole().getPermissions()).flatMap(List::stream).collect(Collectors.toSet());
        if(!permissions.contains(Permission.WRITE_ENTERPRISE_USER_ROLES)) {
            return res.setStatus(Status.ERROR).setMessage("Invalid permissions");
        }
        RolesDetails rolesDetails = rolesService.getEnterpriseRole(dto.getEnterpriseRole());
        if(rolesDetails != null) {
            rolesService.editEnterpriseRole(dto.getEnterpriseRole());
            return res;
        }
        rolesService.createEnterpriseRole(dto.getEnterpriseRole());

        return res;
    }
    
    public AuthenticationOutputDTO removeUserFromEnterprise(AuthenticationInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        Pair<AuthenticationOutputDTO, UserDetailsAuthImplementation> temp = validAuth();
        AuthenticationOutputDTO res = temp.first();
        UserDetailsAuthImplementation details = temp.second();
        EnterpriseDetails enterpriseDetails = (EnterpriseDetails)enterpriseService.loadEnterprisebyName(SecurityContextHolder.getContext().getAuthentication().getPrincipal().toString().split((","))[1]);
        List<UserRoleDTO> roles = details.getRoles().stream().filter(elem -> elem.getEnterpriseId().equals(enterpriseDetails.getEnterprise().getId())).collect(Collectors.toList());
        Set<Permission> permissions = rolesService.getRoles(roles).stream().map(elem -> elem.getEnterpriseRole().getPermissions()).flatMap(List::stream).collect(Collectors.toSet());
        if(!permissions.contains(Permission.WRITE_ENTERPRISE_USERS)) {
            return res.setStatus(Status.ERROR).setMessage("Invalid permissions");
        }

        UserDetailsAuthImplementation subject = (UserDetailsAuthImplementation)userDetailsService.loadUserByUsername(dto.getUser().getUsername());
        if(subject == null) {
            return res.setStatus(Status.ERROR).setMessage("Invalid subject user");
        }
        if(!subject.getUser().getEnabled()) {
            return res.setStatus(Status.ERROR).setMessage("Unverified subject account");
        }
        rolesService.deleteRoleByUserId(subject.getUser().getId());
        userDetailsService.editUser(subject.getUser().setToken(""));
        return res;
    }

    public AuthenticationOutputDTO removeRoleFromEnterprise(AuthenticationInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        Pair<AuthenticationOutputDTO, UserDetailsAuthImplementation> temp = validAuth();
        AuthenticationOutputDTO res = temp.first();
        UserDetailsAuthImplementation details = temp.second();
        EnterpriseDetails enterpriseDetails = (EnterpriseDetails)enterpriseService.loadEnterprisebyName(SecurityContextHolder.getContext().getAuthentication().getPrincipal().toString().split((","))[1]);
        List<UserRoleDTO> roles = details.getRoles();
        roles = roles.stream().filter(elem -> elem.getEnterpriseId().equals(enterpriseDetails.getEnterprise().getId())).collect(Collectors.toList());
        if(roles.size() == 0) {
            return res.setStatus(Status.ERROR).setMessage("Invalid permissions");
        }
        Set<Permission> permissions = roles.stream().map(elem -> rolesService.getRole(elem).getEnterpriseRole().getPermissions()).flatMap(List::stream).collect(Collectors.toSet());
        if(!permissions.contains(Permission.WRITE_ENTERPRISE_USER_ROLES)) {
            return res.setStatus(Status.ERROR).setMessage("Invalid permissions");
        }
        rolesService.deleteEnterpriseRole(dto.getEnterpriseRole());

        return res;
    }

    public AuthenticationOutputDTO validateEnterpriseUser(AuthenticationInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        Pair<AuthenticationOutputDTO, UserDetailsAuthImplementation> temp = validAuth();
        AuthenticationOutputDTO res = temp.first();
        UserDetailsAuthImplementation details = temp.second();
        if(res.getStatus().equals(Status.ERROR)) {
            return res;
        }
        EnterpriseDetails enterpriseDetails = (EnterpriseDetails)enterpriseService.loadEnterprisebyName(SecurityContextHolder.getContext().getAuthentication().getPrincipal().toString().split((","))[1]);
        List<UserRoleDTO> roles = details.getRoles().stream().filter(elem -> elem.getEnterpriseId().equals(enterpriseDetails.getEnterprise().getId())).collect(Collectors.toList());
        Set<Permission> permissions = roles.stream().map(elem -> rolesService.getRole(elem).getEnterpriseRole().getPermissions()).flatMap(List::stream).collect(Collectors.toSet());
        
        return res.setPermissions(permissions).setEnterprise(enterpriseDetails.getEnterprise());
    }
}
