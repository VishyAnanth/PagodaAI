package com.Pagoda.Common.DTOs;

import java.util.HashMap;
import java.util.List;

import lombok.AllArgsConstructor;
import lombok.Getter;
import lombok.NoArgsConstructor;
import lombok.Setter;
import lombok.experimental.Accessors;

@Getter
@Setter
@AllArgsConstructor
@NoArgsConstructor
@Accessors(chain = true)
public class EmailInputDTO {
    private String recipient;
    private String content;
    private Long emailFormatId;
    private HashMap<String, String> emailAttachments;
    private List<String> emailParams;
}
