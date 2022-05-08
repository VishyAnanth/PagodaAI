package com.Pagoda.Projects;

import java.security.SecureRandom;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;
import java.util.Set;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import com.Pagoda.Common.DTOs.*;
import com.Pagoda.Common.Enums.*;
import com.Pagoda.Projects.FeignClients.UserFeignClient;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

@Service
public class EnterpriseVersionControlService {

    @Autowired
    VersionControlRepository versionControlRepository;

    @Autowired
    ProjectsRepository projectsRepository;

    @Autowired
    UserFeignClient userClient;

    public ProjectOutputDTO commitEnterprise(ProjectInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        ProjectOutputDTO res = new ProjectOutputDTO()
        .setStatus(Status.SUCCESS);
        AuthenticationOutputDTO userRes = userClient.validateEnterpriseUser(null);
        if(userRes.getStatus().equals(Status.ERROR)) {
            return res.setStatus(Status.ERROR).setMessage(userRes.getMessage());
        }
        Set<Permission> permissions = userRes.getPermissions();
        if(!permissions.contains(Permission.WRITE_PROJECT_CONTENTS)) {
            return res.setStatus(Status.ERROR).setMessage("Invalid Permissions");
        }
        ProjectDTO actualProject = projectsRepository.retrieveProject(dto.getProject());
        if(!actualProject.getContributors().contains(userRes.getUser().getId())) {
            return res.setStatus(Status.ERROR).setMessage("Invalid ownership credentials");
        }

        List<String> parent = new ArrayList<>();
        if(!actualProject.getHeadCommits().contains(dto.getProject().getHeadCommits().get(0))) {
            return res.setStatus(Status.ERROR).setMessage("Invalid parent commit");
        }
        parent.add(dto.getProject().getHeadCommits().get(0));

        //check if tree exists and shit
        dto.getCommits().get(0)
        .setCommitHash(String.valueOf(new SecureRandom().nextLong()))
        .setAuthorId(userRes.getUser().getId())
        .setCommitDate(new Date(System.currentTimeMillis()))
        .setParents(parent)
        .setTreeId("treeId") //fix this
        .setProjectId(actualProject.getId());

        versionControlRepository.createCommit(dto.getCommits().get(0));

        actualProject.getHeadCommits().set(actualProject.getHeadCommits().indexOf(dto.getProject().getHeadCommits().get(0)), dto.getCommits().get(0).getCommitHash());

        projectsRepository.editProject(actualProject);
        
        List<CommitDTO> commits = new ArrayList<>();
        commits.add(dto.getCommits().get(0));
        return res.setCommits(commits);
    }

    public ProjectOutputDTO branchEnterprise(ProjectInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        ProjectOutputDTO res = new ProjectOutputDTO()
        .setStatus(Status.SUCCESS);
        AuthenticationOutputDTO userRes = userClient.validateEnterpriseUser(null);
        if(userRes.getStatus().equals(Status.ERROR)) {
            return res.setStatus(Status.ERROR).setMessage(userRes.getMessage());
        }
        Set<Permission> permissions = userRes.getPermissions();
        if(!permissions.contains(Permission.WRITE_PROJECT_CONTENTS)) {
            return res.setStatus(Status.ERROR).setMessage("Invalid Permissions");
        }
        ProjectDTO actualProject = projectsRepository.retrieveProject(dto.getProject());
        if(!actualProject.getVisiblePublic() && !actualProject.getVisibleEnterprise() && 
            !actualProject.getContributors().contains(userRes.getUser().getId()) &&
            !actualProject.getViewers().contains(userRes.getUser().getId())) {
            return res.setStatus(Status.ERROR).setMessage("Invalid ownership credentials");
        }
        if(actualProject.getVisibleEnterprise() && actualProject.getOwnerId() != userRes.getEnterprise().getId()) {
            return res.setStatus(Status.ERROR).setMessage("Invalid ownership credentials");
        }

        CommitDTO actualCommit = versionControlRepository.retreiveCommit(dto.getCommits().get(0));
        if(actualCommit == null || actualCommit.getProjectId() != actualProject.getId()) {
            return res.setStatus(Status.ERROR).setMessage("Invalid commit");
        }
        List<String> parent = new ArrayList<>();
        parent.add(actualCommit.getCommitHash());
        List<CommitDTO> commits = new ArrayList<>();
       
        CommitDTO newCommit1 = new CommitDTO()
        .setAuthorId(userRes.getUser().getId())
        .setCommitDate(new Date(System.currentTimeMillis()))
        .setCommitHash(String.valueOf(new SecureRandom().nextLong()))
        .setDescription("Commit " + actualCommit.getCommitHash() + " was branched at the head")
        .setParents(parent)
        .setProjectId(actualProject.getId())
        .setTreeId("tree");

        versionControlRepository.createCommit(newCommit1);
        actualProject.getHeadCommits().add(newCommit1.getCommitHash());
        commits.add(newCommit1);

        if(actualProject.getHeadCommits().contains(actualCommit.getCommitHash())) {
            CommitDTO newCommit2 = new CommitDTO()
            .setAuthorId(userRes.getUser().getId())
            .setCommitDate(new Date(System.currentTimeMillis()))
            .setCommitHash(String.valueOf(new SecureRandom().nextLong()))
            .setDescription("Commit " + actualCommit.getCommitHash() + " was branched at the head")
            .setParents(parent)
            .setProjectId(actualProject.getId())
            .setTreeId("tree");

            versionControlRepository.createCommit(newCommit2);

            actualProject.getHeadCommits().remove(dto.getProject().getHeadCommits().get(0));
            actualProject.getHeadCommits().add(newCommit2.getCommitHash());
            commits.add(newCommit2);
        }
        projectsRepository.editProject(actualProject);
       
        return res.setCommits(commits);
    }

    public ProjectOutputDTO mergeEnterprise(ProjectInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        ProjectOutputDTO res = new ProjectOutputDTO()
        .setStatus(Status.SUCCESS);
        AuthenticationOutputDTO userRes = userClient.validateEnterpriseUser(null);
        if(userRes.getStatus().equals(Status.ERROR)) {
            return res.setStatus(Status.ERROR).setMessage(userRes.getMessage());
        }
        Set<Permission> permissions = userRes.getPermissions();
        if(!permissions.contains(Permission.WRITE_PROJECT_CONTENTS)) {
            return res.setStatus(Status.ERROR).setMessage("Invalid Permissions");
        }
        ProjectDTO actualProject = projectsRepository.retrieveProject(dto.getProject());
        if(!actualProject.getContributors().contains(userRes.getUser().getId())) {
            return res.setStatus(Status.ERROR).setMessage("Invalid ownership credentials");
        }
        CommitDTO commit1 = versionControlRepository.retreiveCommit(dto.getCommits().get(0));
        CommitDTO commit2 = versionControlRepository.retreiveCommit(dto.getCommits().get(1));
        if(commit1.getProjectId() != actualProject.getId() || commit2.getProjectId() != actualProject.getId()) {
            return res.setStatus(Status.ERROR).setMessage("Invalid commit");
        }
        List<String> parents = new ArrayList<>();
        parents.add(commit1.getCommitHash());
        parents.add(commit2.getCommitHash());

        CommitDTO newCommit = new CommitDTO()
        .setAuthorId(userRes.getUser().getId())
        .setCommitDate(new Date(System.currentTimeMillis()))
        .setCommitHash(String.valueOf(new SecureRandom().nextLong()))
        .setDescription("Commit " + commit1.getCommitHash() + " and " + commit2.getCommitHash() + " was merged at the head")
        .setParents(parents)
        .setProjectId(actualProject.getId())
        .setTreeId("tree");
        versionControlRepository.createCommit(newCommit);

        actualProject.getHeadCommits().remove(commit1.getCommitHash());
        actualProject.getHeadCommits().remove(commit2.getCommitHash());
        actualProject.getHeadCommits().add(newCommit.getCommitHash());
        projectsRepository.editProject(actualProject);
        
        List<CommitDTO> commits = new ArrayList<>();
        commits.add(newCommit);
        return res.setCommits(commits);
    }

    public ProjectOutputDTO retrieveCommitEnterprise(ProjectInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        ProjectOutputDTO res = new ProjectOutputDTO()
        .setStatus(Status.SUCCESS);
        AuthenticationOutputDTO userRes = userClient.validateEnterpriseUser(null);
        if(userRes.getStatus().equals(Status.ERROR)) {
            return res.setStatus(Status.ERROR).setMessage(userRes.getMessage());
        }
        Set<Permission> permissions = userRes.getPermissions();
        if(!permissions.contains(Permission.READ_PROJECT_CONTENTS)) {
            return res.setStatus(Status.ERROR).setMessage("Invalid Permissions");
        }
        ProjectDTO actualProject = projectsRepository.retrieveProject(dto.getProject());
        if(!actualProject.getVisiblePublic() && !actualProject.getVisibleEnterprise() &&
            !actualProject.getContributors().contains(userRes.getUser().getId()) &&
            !actualProject.getViewers().contains(userRes.getUser().getId())) {
            return res.setStatus(Status.ERROR).setMessage("Invalid ownership credentials");
        }
        if(actualProject.getVisibleEnterprise() && actualProject.getId() != userRes.getEnterprise().getId()) {
            return res.setStatus(Status.ERROR).setMessage("Invalid ownership credentials");
        }
        CommitDTO actualCommit = versionControlRepository.retreiveCommit(dto.getCommits().get(0));
        if(actualCommit.getProjectId() != actualProject.getId()) {
            return res.setStatus(Status.ERROR).setMessage("Invalid commit");
        }
        List<CommitDTO> commits = new ArrayList<>();
        commits.add(actualCommit);
        return res.setCommits(commits);
    }

    public ProjectOutputDTO retrieveCommitsEnterprise(ProjectInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        ProjectOutputDTO res = new ProjectOutputDTO()
        .setStatus(Status.SUCCESS);
        AuthenticationOutputDTO userRes = userClient.validateEnterpriseUser(null);
        if(userRes.getStatus().equals(Status.ERROR)) {
            return res.setStatus(Status.ERROR).setMessage(userRes.getMessage());
        }
        Set<Permission> permissions = userRes.getPermissions();
        if(!permissions.contains(Permission.READ_PROJECT_CONTENTS)) {
            return res.setStatus(Status.ERROR).setMessage("Invalid Permissions");
        }
        ProjectDTO actualProject = projectsRepository.retrieveProject(dto.getProject());
        if(!actualProject.getVisiblePublic() && !actualProject.getVisibleEnterprise() && 
            !actualProject.getContributors().contains(userRes.getUser().getId()) &&
            !actualProject.getViewers().contains(userRes.getUser().getId())) {
            return res.setStatus(Status.ERROR).setMessage("Invalid ownership credentials");
        }
        if(actualProject.getVisibleEnterprise() && actualProject.getId() != userRes.getEnterprise().getId()) {
            return res.setStatus(Status.ERROR).setMessage("Invalid ownership credentials");
        }
        CommitDTO actualCommit = versionControlRepository.retreiveCommit(dto.getCommits().get(0));
        if(actualCommit.getProjectId() != actualProject.getId()) {
            return res.setStatus(Status.ERROR).setMessage("Invalid commit");
        }
        List<CommitDTO> commits = versionControlRepository.retreiveCommits(actualCommit);
        return res.setCommits(commits);
    }

}