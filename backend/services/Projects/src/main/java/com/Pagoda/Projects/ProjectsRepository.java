package com.Pagoda.Projects;

import java.util.List;

import com.Pagoda.Common.DTOs.ProjectDTO;
import com.Pagoda.Common.DTOs.StarDTO;
import com.Pagoda.Common.DTOs.TagDTO;

import org.springframework.stereotype.Repository;

@Repository
public class ProjectsRepository {
    
    public ProjectDTO createProject(ProjectDTO project) {
        return null;
    }

    public ProjectDTO editProject(ProjectDTO project) {
        return null;
    }

    public ProjectDTO deleteProject(ProjectDTO project) {
        return null;
    }

    public ProjectDTO retrieveProject(ProjectDTO project) {
        return null;
    } 

    public List<ProjectDTO> searchProjects(ProjectDTO project) {
        return null;
    }

    public StarDTO starProject(ProjectDTO project, Long userId) {
        return null;
    }

    public StarDTO retrieveStar(ProjectDTO project, Long userId) {
        return null;
    }

    public List<ProjectDTO> retrieveForked(ProjectDTO project) {
        return null;
    }

    public List<TagDTO> retrieveTags(ProjectDTO project) {
        return null;
    }
    public List<TagDTO> retrieveTagsById(List<Long> ids) {
        return null;
    }
}
