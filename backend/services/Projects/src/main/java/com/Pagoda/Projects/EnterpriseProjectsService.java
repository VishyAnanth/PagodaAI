package com.Pagoda.Projects;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import com.Pagoda.Common.DTOs.*;
import com.Pagoda.Common.Enums.*;
import com.Pagoda.Projects.FeignClients.UserFeignClient;

import java.security.SecureRandom;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.stream.Collectors;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.security.core.context.SecurityContextHolder;
import org.springframework.stereotype.Service;

@Service
public class EnterpriseProjectsService {

    @Autowired
    ProjectsRepository projectsRepository;

    @Autowired
    UserFeignClient userClient;

    @Autowired
    VersionControlRepository versionControlRepository;
    
    public ProjectOutputDTO createEnterpriseProject(ProjectInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        ProjectOutputDTO res = new ProjectOutputDTO()
        .setStatus(Status.SUCCESS);

        AuthenticationOutputDTO userRes = userClient.validateEnterpriseUser(null);
        if(userRes.getStatus().equals(Status.ERROR)) {
            return res.setStatus(Status.ERROR).setMessage(userRes.getMessage());
        }
        Set<Permission> permissions = userRes.getPermissions();
        if(!permissions.contains(Permission.WRITE_PROJECT_METADATA)) {
            return res.setStatus(Status.ERROR).setMessage("Invalid Permissions");
        }
        if(dto.getProject().getApiPrice() < 0) {
            return res.setStatus(Status.ERROR).setMessage("Invalid API Price");
        }
        
        List<Long> contributors = new ArrayList<>();
        List<Long> viewers = new ArrayList<>();
        if(dto.getContributors().size() == 0) {
            contributors.add(userRes.getUser().getId());
        } else {
            contributors = userClient.getUsersDetails(dto.getContributors()).stream().
                map(elem -> elem.getUser().getId()).collect(Collectors.toList());
            contributors.add(userRes.getUser().getId());
            Set<Long> tempSet = new HashSet<>(contributors);
            contributors.clear();
            contributors.addAll(tempSet);
        }

        if(dto.getViewers().size() == 0 && dto.getProject().getVisiblePublic() == false) {
            viewers.add(userRes.getUser().getId());
        } else if(dto.getProject().getVisiblePublic() == false) {
            viewers = userClient.getUsersDetails(dto.getViewers()).stream()
                .map(elem -> elem.getUser().getId()).collect(Collectors.toList());
            viewers.add(userRes.getUser().getId());
            Set<Long> tempSet = new HashSet<>(viewers);
            viewers.clear();
            viewers.addAll(tempSet);
        }
        
        CommitDTO initialCommit = new CommitDTO()
        .setCommitHash(String.valueOf(new SecureRandom().nextLong()))
        .setAuthorId(userRes.getUser().getId())
        .setCommitDate(new Date(System.currentTimeMillis()))
        .setDescription("Create a new Pagoda project called " + dto.getProject().getName() + ". This project serves the following purpose. " + dto.getProject().getDescription())
        .setParents(null)
        .setTreeId("treeId");//FIX THIS (SEND THIS TO THE VERSION CONTROL MICROSERVICE)

        List<String> headCommits = new ArrayList<>();
        headCommits.add(initialCommit.getCommitHash());

        dto.getProject()
        .setOwnerId(userRes.getEnterprise().getId())
        .setStars(Long.valueOf(0))
        .setForks(null)
        .setContributors(contributors)
        .setViewers(viewers)
        .setId(new SecureRandom().nextLong())
        .setHeadCommits(headCommits)
        .setEnterpriseOwner(true);

        projectsRepository.createProject(dto.getProject());

        initialCommit.setProjectId(dto.getProject().getId());
        initialCommit = versionControlRepository.createCommit(initialCommit);

        List<ProjectDTO> projects = new ArrayList<>();
        projects.add(dto.getProject().clearSensistiveInfo());
        List<CommitDTO> commits = new ArrayList<>();
        commits.add(initialCommit);
        return res.setProjects(projects).setCommits(commits);
    }

    public ProjectOutputDTO editEnterpriseProject(ProjectInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        ProjectOutputDTO res = new ProjectOutputDTO()
        .setStatus(Status.SUCCESS);

        Boolean permissionCheck = true;
        AuthenticationOutputDTO userRes = userClient.validateEnterpriseUser(null);
        if(userRes.getStatus().equals(Status.ERROR)) {
            userRes = userClient.getUserDetails(SecurityContextHolder.getContext().getAuthentication().getPrincipal().toString());
            if(userRes.getStatus() == Status.ERROR) {
                return res.setStatus(Status.ERROR).setMessage(userRes.getMessage());
            }
            permissionCheck = false;
        }
        if(permissionCheck) {
            Set<Permission> permissions = userRes.getPermissions();
            if(!permissions.contains(Permission.WRITE_PROJECT_METADATA)) {
                return res.setStatus(Status.ERROR).setMessage("Invalid Permissions");
            }
        }
        if(dto.getProject().getApiPrice() < 0) {
            return res.setStatus(Status.ERROR).setMessage("Invalid API Price");
        }
        ProjectDTO actualProj = projectsRepository.retrieveProject(dto.getProject());
        if(actualProj == null) {
            return res.setStatus(Status.ERROR).setMessage("Project not found");
        }
        if(!actualProj.getContributors().contains(userRes.getUser().getId())) {
            return res.setStatus(Status.ERROR).setMessage("Invalid ownership credentials");
        }
        if(dto.getProject().getContributors().size() < 1 || !dto.getProject().getContributors().contains(actualProj.getOwnerId())) {
            return res.setStatus(Status.ERROR).setMessage("Invalid contributors");
        }

        List<Long> contributors = new ArrayList<>();
        if(dto.getContributors().size() != 0) {
            if(dto.getAddChanges()) {
                contributors = userClient.getUsersDetails(dto.getContributors()).stream().
                map(elem -> elem.getUser().getId()).collect(Collectors.toList());
                contributors.addAll(actualProj.getContributors());
                Set<Long> tempSet = new HashSet<>(contributors);
                contributors.clear();
                contributors.addAll(tempSet);
            } else {
                contributors = userClient.getUsersDetails(dto.getContributors()).stream().
                map(elem -> elem.getUser().getId()).collect(Collectors.toList());
                actualProj.getContributors().removeAll(contributors);
                actualProj.getContributors().add(actualProj.getOwnerId());
                contributors = actualProj.getContributors();
            }
        } else {
            contributors = actualProj.getContributors();
        }

        List<Long> viewers = new ArrayList<>();
        if(dto.getViewers().size() != 0) {
            if(dto.getAddChanges()) {
                viewers = userClient.getUsersDetails(dto.getViewers()).stream().
                map(elem -> elem.getUser().getId()).collect(Collectors.toList());
                viewers.addAll(actualProj.getViewers());
                Set<Long> tempSet = new HashSet<>(viewers);
                viewers.clear();
                viewers.addAll(tempSet);
            } else {
                viewers = userClient.getUsersDetails(dto.getViewers()).stream().
                map(elem -> elem.getUser().getId()).collect(Collectors.toList());
                actualProj.getViewers().removeAll(viewers);
                actualProj.getViewers().add(actualProj.getOwnerId());
                viewers = actualProj.getViewers();
            }
        } else {
            viewers = actualProj.getViewers();
        }
        dto.getProject()
        .setId(actualProj.getId())
        .setForks(actualProj.getForks())
        .setHeadCommits(actualProj.getHeadCommits())
        .setOwnerId(actualProj.getOwnerId())
        .setStars(actualProj.getStars())
        .setViewers(viewers)
        .setContributors(contributors)
        .setEnterpriseOwner(true);

        projectsRepository.editProject(dto.getProject());

        List<ProjectDTO> projects = new ArrayList<>();
        projects.add(dto.getProject().clearSensistiveInfo());

        return res.setProjects(projects);
    }

    public ProjectOutputDTO forkEnterpriseProject(ProjectInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        ProjectOutputDTO res = new ProjectOutputDTO()
        .setStatus(Status.SUCCESS);

        Boolean permissionCheck = true;
        AuthenticationOutputDTO userRes = userClient.validateEnterpriseUser(null);
        if(userRes.getStatus().equals(Status.ERROR)) {
            userRes = userClient.getUserDetails(SecurityContextHolder.getContext().getAuthentication().getPrincipal().toString());
            if(userRes.getStatus() == Status.ERROR) {
                return res.setStatus(Status.ERROR).setMessage(userRes.getMessage());
            }
            permissionCheck = false;
        }
        if(permissionCheck) {
            Set<Permission> permissions = userRes.getPermissions();
            if(!permissions.contains(Permission.WRITE_PROJECT_METADATA) && !permissions.contains(Permission.READ_PROJECT_METADATA)) {
                return res.setStatus(Status.ERROR).setMessage("Invalid Permissions");
            }   
        }
        ProjectDTO newProj = projectsRepository.retrieveProject(dto.getProject());
        CommitDTO projCommit = versionControlRepository.retreiveCommit(dto.getCommits().get(0));
        if(newProj == null) {
            return res.setStatus(Status.ERROR).setMessage("Project not found");
        }
        if(!newProj.getAllowForks() ||
            (!newProj.getVisibleEnterprise() && permissionCheck && !newProj.getViewers().contains(userRes.getUser().getId()) && !newProj.getContributors().contains(userRes.getUser().getId())) ||
            (!newProj.getVisiblePublic() && !newProj.getViewers().contains(userRes.getUser().getId()) && !newProj.getContributors().contains(userRes.getUser().getId()))) {
            
            return res.setStatus(Status.ERROR).setMessage("Forks not allowed for this project");
        }
        if(dto.getCommits().size() == 0 || projCommit.getProjectId() != newProj.getId()) {
            return res.setStatus(Status.ERROR).setMessage("Invalid commit");
        }

        List<Long> contributors = new ArrayList<>();
        contributors.add(userRes.getUser().getId());

        List<Long> viewers = newProj.getViewers();
        if(!newProj.getVisiblePublic()) {
            viewers.clear();
            viewers.add(userRes.getUser().getId());
        }

        List<String> initialCommits = new ArrayList<>();
        initialCommits.add(projCommit.getCommitHash());

        CommitDTO initialCommit = new CommitDTO()
        .setCommitHash(String.valueOf(new SecureRandom().nextLong()))
        .setAuthorId(userRes.getUser().getId())
        .setCommitDate(new Date(System.currentTimeMillis()))
        .setDescription("Forked a Pagoda project called " + newProj.getName())
        .setParents(initialCommits)
        .setTreeId(projCommit.getTreeId()); 

        List<String> headCommits = new ArrayList<>();
        headCommits.add(initialCommit.getCommitHash());

        newProj
        .setContributors(contributors)
        .setViewers(viewers)
        .setForks(null)
        .setId(new SecureRandom().nextLong())
        .setStars(Long.valueOf(0))
        .setHeadCommits(headCommits)
        .setEnterpriseOwner(true);

        if(!permissionCheck) {
            newProj.setOwnerId(userRes.getUser().getId());
        }

        projectsRepository.createProject(newProj);
        initialCommit.setProjectId(newProj.getId());
        initialCommit = versionControlRepository.createCommit(initialCommit);

        List<ProjectDTO> projects = new ArrayList<>();
        projects.add(newProj.clearSensistiveInfo());
        List<CommitDTO> commits = new ArrayList<>();
        commits.add(initialCommit);

        return res.setProjects(projects).setCommits(commits);
    }

    public ProjectOutputDTO deleteEnterpriseProject(ProjectInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        ProjectOutputDTO res = new ProjectOutputDTO()
        .setStatus(Status.SUCCESS);

        AuthenticationOutputDTO userRes = userClient.validateEnterpriseUser(null);
        if(userRes.getStatus().equals(Status.ERROR)) {
            return res.setStatus(Status.ERROR).setMessage(userRes.getMessage());
        }
        Set<Permission> permissions = userRes.getPermissions();
        if(!permissions.contains(Permission.WRITE_PROJECT_METADATA)) {
            return res.setStatus(Status.ERROR).setMessage("Invalid Permissions");
        }
        ProjectDTO actualProject = projectsRepository.retrieveProject(dto.getProject());
        if(actualProject == null) {
            return res.setStatus(Status.ERROR).setMessage("Project not found");
        }
        if(!actualProject.getContributors().contains(userRes.getUser().getId())) {
            return res.setStatus(Status.ERROR).setMessage("Invalid ownership credentials");
        }
        versionControlRepository.deleteCommitsByProjectId(actualProject.getId());
        projectsRepository.deleteProject(actualProject);
        return res;
    }

    public ProjectOutputDTO retrieveEnterpriseProject(ProjectInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        ProjectOutputDTO res = new ProjectOutputDTO()
        .setStatus(Status.SUCCESS);
        Boolean permissionCheck = true;
        AuthenticationOutputDTO userRes = userClient.validateEnterpriseUser(null);
        if(userRes.getStatus().equals(Status.ERROR)) {
            userRes = userClient.getUserDetails(SecurityContextHolder.getContext().getAuthentication().getPrincipal().toString());
            if(userRes.getStatus() == Status.ERROR) {
                return res.setStatus(Status.ERROR).setMessage(userRes.getMessage());
            }
            permissionCheck = false;
        }
        if(permissionCheck) {
            Set<Permission> permissions = userRes.getPermissions();
            if(!permissions.contains(Permission.READ_PROJECT_METADATA)) {
                return res.setStatus(Status.ERROR).setMessage("Invalid Permissions");
            }   
        }
        ProjectDTO actualProject = projectsRepository.retrieveProject(dto.getProject());
        if(actualProject == null) {
            return res.setStatus(Status.ERROR).setMessage("Project not found");
        }
        if(!permissionCheck && !actualProject.getVisiblePublic() && actualProject.getOwnerId() != userRes.getUser().getId() && 
            !actualProject.getContributors().contains(userRes.getUser().getId()) &&
            !actualProject.getViewers().contains(userRes.getUser().getId())) {
            return res.setStatus(Status.ERROR).setMessage("Invalid ownership credentials");
        } else if(permissionCheck && !actualProject.getVisibleEnterprise() && actualProject.getOwnerId() != userRes.getEnterprise().getId()
        && !actualProject.getContributors().contains(userRes.getUser().getId()) &&
        !actualProject.getViewers().contains(userRes.getUser().getId())) {
            return res.setStatus(Status.ERROR).setMessage("Invalid ownership credentials");
        }
        List<TagDTO> tags = projectsRepository.retrieveTags(actualProject);
        List<ProjectDTO> projects = new ArrayList<>();
        projects.add(actualProject.clearSensistiveInfo());
        return res.setProjects(projects).setTags(tags);
    }

    public ProjectOutputDTO searchEntepriseProjects(ProjectInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        ProjectOutputDTO res = new ProjectOutputDTO()
        .setStatus(Status.SUCCESS);
        AuthenticationOutputDTO userRes = userClient.validateEnterpriseUser(null);
        if(userRes.getStatus().equals(Status.ERROR)) {
            return res.setStatus(Status.ERROR).setMessage(userRes.getMessage());
        }
        Set<Permission> permissions = userRes.getPermissions();
        if(!permissions.contains(Permission.READ_PROJECT_METADATA)) {
            return res.setStatus(Status.ERROR).setMessage("Invalid Permissions");
        }

        List<ProjectDTO> results = projectsRepository.searchProjects(dto.getProject());
        results = results.stream()
        .filter(result ->
            (
                (result.getVisibleEnterprise() || 
                result.getVisiblePublic() || 
                result.getContributors().contains(userRes.getUser().getId()) || 
                result.getViewers().contains(userRes.getUser().getId()))
                && result.getEnterpriseOwner() 
                && result.getOwnerId() == userRes.getEnterprise().getId()
            )
        )
        .map(elem -> elem.clearSensistiveInfo())
        .collect(Collectors.toList());
        return res.setProjects(results);
    }

    public ProjectOutputDTO starEnterpriseProject(ProjectInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        ProjectOutputDTO res = new ProjectOutputDTO()
        .setStatus(Status.SUCCESS);
        AuthenticationOutputDTO userRes = userClient.validateEnterpriseUser(null);
        if(userRes.getStatus().equals(Status.ERROR)) {
            return res.setStatus(Status.ERROR).setMessage(userRes.getMessage());
        }
        ProjectDTO actualProject = projectsRepository.retrieveProject(dto.getProject());
        if(actualProject == null) {
            return res.setStatus(Status.ERROR).setMessage("Invalid Project");
        }
        if((!actualProject.getVisiblePublic() &&
            !actualProject.getVisibleEnterprise() &&
            !actualProject.getContributors().contains(userRes.getUser().getId()) &&
            !actualProject.getViewers().contains(userRes.getUser().getId())) ||
            (actualProject.getVisibleEnterprise() && actualProject.getOwnerId() != userRes.getEnterprise().getId())) {
                return res.setStatus(Status.ERROR).setMessage("Invalid ownership credentials");
        }
        StarDTO star = projectsRepository.retrieveStar(actualProject, userRes.getUser().getId());
        if(star != null) {
            return res.setStatus(Status.ERROR).setMessage("Already starred");
        }
        projectsRepository.starProject(actualProject, userRes.getUser().getId());
        actualProject.setStars(actualProject.getStars() + 1);
        projectsRepository.editProject(actualProject);
        return res.setStatus(Status.SUCCESS);
    }

    public ProjectOutputDTO getEnterpriseForks(ProjectInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        ProjectOutputDTO res = new ProjectOutputDTO()
        .setStatus(Status.SUCCESS);
        AuthenticationOutputDTO userRes = userClient.validateEnterpriseUser(null);
        if(userRes.getStatus().equals(Status.ERROR)) {
            return res.setStatus(Status.ERROR).setMessage(userRes.getMessage());
        }
        Set<Permission> permissions = userRes.getPermissions();
        if(!permissions.contains(Permission.READ_PROJECT_METADATA)) {
            return res.setStatus(Status.ERROR).setMessage("Invalid Permissions");
        }
        ProjectDTO actualProject = projectsRepository.retrieveProject(dto.getProject());
        if(actualProject == null) {
            return res.setStatus(Status.ERROR).setMessage("Project not found");
        }
        if((!actualProject.getVisiblePublic() &&
            !actualProject.getVisibleEnterprise() &&
            !actualProject.getContributors().contains(userRes.getUser().getId()) &&
            !actualProject.getViewers().contains(userRes.getUser().getId())) ||
            (actualProject.getVisibleEnterprise() && actualProject.getOwnerId() != userRes.getEnterprise().getId())) {
                return res.setStatus(Status.ERROR).setMessage("Invalid ownership credentials");
        }
        List<ProjectDTO> resList = projectsRepository.retrieveForked(actualProject);
        resList = resList.stream()
        .filter(
            result ->
            (
                result.getVisiblePublic() ||
                result.getContributors().contains(userRes.getUser().getId()) || 
                result.getViewers().contains(userRes.getUser().getId()) || 
                (result.getVisibleEnterprise() && result.getOwnerId() == userRes.getEnterprise().getId())
            )
        )
        .map(elem -> elem.clearSensistiveInfo())
        .collect(Collectors.toList());
        return res.setProjects(resList);
    }

    public ProjectOutputDTO getEnterpriseTags(ProjectInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        ProjectOutputDTO res = new ProjectOutputDTO()
        .setStatus(Status.SUCCESS);
        AuthenticationOutputDTO userRes = userClient.validateEnterpriseUser(null);
        if(userRes.getStatus().equals(Status.ERROR)) {
            return res.setStatus(Status.ERROR).setMessage(userRes.getMessage());
        }
        Set<Permission> permissions = userRes.getPermissions();
        if(!permissions.contains(Permission.READ_PROJECT_METADATA)) {
            return res.setStatus(Status.ERROR).setMessage("Invalid Permissions");
        }
        ProjectDTO actualProject = projectsRepository.retrieveProject(dto.getProject());
        if(actualProject == null) {
            return res.setStatus(Status.ERROR).setMessage("Invalid project");
        }
        if(actualProject.getOwnerId() != userRes.getEnterprise().getId()) {
            return res.setStatus(Status.ERROR).setMessage("Invalid Ownership Credentials");
        }
        return res.setTags(projectsRepository.retrieveTagsById(dto.getProject().getTags()));
    }
 
}
