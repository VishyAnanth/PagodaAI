package com.Pagoda.Common.DTOs;

import java.util.List;

import lombok.*;
import lombok.experimental.Accessors;

@Getter
@Setter
@NoArgsConstructor
@AllArgsConstructor
@Accessors(chain = true)
public class ContainerInputDTO {
    private List<ContainerDTO> containers;
}