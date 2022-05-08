package com.Pagoda.Authorization.DetailsServices;

import org.springframework.security.core.userdetails.UserDetailsService;
import org.springframework.security.core.userdetails.UserDetails;
import java.util.List;
import java.util.ArrayList;
import org.springframework.stereotype.Service;

import com.Pagoda.Common.DTOs.*;

@Service
public class UserDetailsAuthService implements UserDetailsService {
    
    @Override
    public UserDetails loadUserByUsername(String username) {
        //get the user from the database

        UserDTO userDto = new UserDTO();
        List<UserRoleDTO> roles = new ArrayList<>();
        return UserDetailsAuthImplementation.build(userDto, roles);
    }       

    public UserDetails loadUserByEmail(String email) {
        //get the user from the database

        UserDTO userDto = new UserDTO();
        List<UserRoleDTO> roles = new ArrayList<>();
        return UserDetailsAuthImplementation.build(userDto, roles);
    }

    public UserDetails loadUserById(String id) {
        //get the user from the database

        UserDTO userDto = new UserDTO();
        List<UserRoleDTO> roles = new ArrayList<>();
        return UserDetailsAuthImplementation.build(userDto, roles);
    }

    public UserDetails editUser(UserDTO user) {
        //push edit to database

        List<UserRoleDTO> roles = new ArrayList<>();
        return UserDetailsAuthImplementation.build(user, roles);
    }

    public UserDetails editUserToUser(UserDTO currentUser, UserDTO destinationUser) {
        return null;
    }

    public UserDetails createUser(UserDTO user) {
        //create user here;
        //remember to reset boolean values
        List<UserRoleDTO> roles = new ArrayList<>();
        return UserDetailsAuthImplementation.build(user, roles);
    }

    public void deleteUser(UserDTO user) {
        //delete the user and roles;
    }

    public List<UserDetails> loadUsersByUsername(List<String> usernames) {
        return null;
    }

    public List<UserDetails> searchUserByUsernames(List<String> usernames) {
        return null;
    }

    public List<UserDetails> searchUserByFirstName(List<String> firstNames) {
        return null;
    }

    public List<UserDetails> searchUserByLastName(List<String> lastNames) {
        return null;
    }
    
    public List<UserDetails> searchUserByEmail(List<String> emails) {
        return null;
    }

    public List<EnterpriseDTO> loadEnterprisesByUserUsername(String username) {
        return null;
    }
}