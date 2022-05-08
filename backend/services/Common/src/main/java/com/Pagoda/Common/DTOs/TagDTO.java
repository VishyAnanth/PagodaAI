package com.Pagoda.Common.DTOs;

import lombok.*;
import lombok.experimental.Accessors;

@Getter
@Setter
@NoArgsConstructor
@AllArgsConstructor
@Accessors(chain = true)
public class TagDTO {
    private Long tagId;
    private String tagName;
    private String tagIcon;
}
