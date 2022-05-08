package com.Pagoda.Projects;

import java.util.List;

import com.Pagoda.Common.DTOs.*;

import org.springframework.stereotype.Repository;

@Repository
public class VersionControlRepository {
    public CommitDTO createCommit(CommitDTO commit) {
        return null;
    }

    public CommitDTO mergeBranches(CommitDTO commit) {
        return null;
    }

    public CommitDTO retreiveCommit(CommitDTO commit) {
        return null;
    }

    public List<CommitDTO> retreiveCommits(CommitDTO commit) {
        return null;
    }

    public void deleteCommit(CommitDTO commit) {

    }

    public void deleteCommitsByProjectId(Long id) {
        
    }
}
