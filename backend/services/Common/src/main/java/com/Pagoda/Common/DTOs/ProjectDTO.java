package com.Pagoda.Common.DTOs;

import java.util.List;

import lombok.*;
import lombok.experimental.Accessors;

@Getter
@Setter
@NoArgsConstructor
@AllArgsConstructor
@Accessors(chain = true)
public class ProjectDTO {
    private Long id;
    private Long ownerId; //this can be an enterprise or individual
    private Boolean enterpriseOwner;
    private List<String> trees; //this is an id for storing trees in a DB for retrieval later
    private List<String> developmentTaskResourceName;
    private List<String> deploymentTaskResourceNames;
    private String name;
    private String description;
    private String wiki;
    private Integer apiPrice; //in US cents unless J Powell keeps printing
    private List<String> headCommits;
    private Boolean visiblePublic;
    private Boolean visibleEnterprise;
    private Boolean allowForks;
    private Long stars;    
    private List<Long> contributors;
    private List<Long> viewers; //if not public, can specify people who can view
    private List<Long> tags;
    private List<Long> forks;
    private Long source;

    public ProjectDTO clearSensistiveInfo() {
        this.developmentTaskResourceName = null;
        this.deploymentTaskResourceNames = null;
        return this;
    }
}

