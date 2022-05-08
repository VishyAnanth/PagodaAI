package com.Pagoda.Common.DTOs;

import java.util.List;

import ch.qos.logback.core.status.Status;
import lombok.*;
import lombok.experimental.Accessors;

@Getter
@Setter
@AllArgsConstructor
@NoArgsConstructor
@Accessors(chain = true)
public class ContainerOutputDTO {
    private Status status;
    private List<ContainerDTO> containers;
}
