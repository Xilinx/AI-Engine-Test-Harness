% MIT License
%
% Copyright (C) 2024-2025 Advanced Micro Devices, Inc.
%
% Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
%
% The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
%
% THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
%
% Except as contained in this notice, the name of Advanced Micro Devices, Inc. shall not be used in advertising or otherwise to promote the sale, use or other dealings in this Software without prior written authorization from Advanced Micro Devices, Inc.
%

classdef channel_index < uint16
    enumeration
        PLIO_01_TO_AIE (0),
        PLIO_02_TO_AIE (1),
        PLIO_03_TO_AIE (2),
        PLIO_04_TO_AIE (3),
        PLIO_05_TO_AIE (4),
        PLIO_06_TO_AIE (5),
        PLIO_07_TO_AIE (6),
        PLIO_08_TO_AIE (7),
        PLIO_09_TO_AIE (8),
        PLIO_10_TO_AIE (9),
        PLIO_11_TO_AIE (10),
        PLIO_12_TO_AIE (11),
        PLIO_13_TO_AIE (12),
        PLIO_14_TO_AIE (13),
        PLIO_15_TO_AIE (14),
        PLIO_16_TO_AIE (15),
        PLIO_17_TO_AIE (16),
        PLIO_18_TO_AIE (17),
        PLIO_19_TO_AIE (18),
        PLIO_20_TO_AIE (19),
        PLIO_21_TO_AIE (20),
        PLIO_22_TO_AIE (21),
        PLIO_23_TO_AIE (22),
        PLIO_24_TO_AIE (23),
        PLIO_25_TO_AIE (24),
        PLIO_26_TO_AIE (25),
        PLIO_27_TO_AIE (26),
        PLIO_28_TO_AIE (27),
        PLIO_29_TO_AIE (28),
        PLIO_30_TO_AIE (39),
        PLIO_31_TO_AIE (30),
        PLIO_32_TO_AIE (31),
        PLIO_33_TO_AIE (32),
        PLIO_34_TO_AIE (33),
        PLIO_35_TO_AIE (34),
        PLIO_36_TO_AIE (35),
        PLIO_01_FROM_AIE (36),
        PLIO_02_FROM_AIE (37),
        PLIO_03_FROM_AIE (38),
        PLIO_04_FROM_AIE (39),
        PLIO_05_FROM_AIE (40),
        PLIO_06_FROM_AIE (41),
        PLIO_07_FROM_AIE (42),
        PLIO_08_FROM_AIE (43),
        PLIO_09_FROM_AIE (44),
        PLIO_10_FROM_AIE (45),
        PLIO_11_FROM_AIE (46),
        PLIO_12_FROM_AIE (47),
        PLIO_13_FROM_AIE (48),
        PLIO_14_FROM_AIE (49),
        PLIO_15_FROM_AIE (50),
        PLIO_16_FROM_AIE (51),
        PLIO_17_FROM_AIE (52),
        PLIO_18_FROM_AIE (53),
        PLIO_19_FROM_AIE (54),
        PLIO_20_FROM_AIE (55),
        PLIO_21_FROM_AIE (56),
        PLIO_22_FROM_AIE (57),
        PLIO_23_FROM_AIE (58),
        PLIO_24_FROM_AIE (59),
        PLIO_25_FROM_AIE (60),
        PLIO_26_FROM_AIE (61),
        PLIO_27_FROM_AIE (62),
        PLIO_28_FROM_AIE (63),
        PLIO_29_FROM_AIE (64),
        PLIO_30_FROM_AIE (65),
        PLIO_31_FROM_AIE (66),
        PLIO_32_FROM_AIE (67),
        PLIO_33_FROM_AIE (68),
        PLIO_34_FROM_AIE (69),
        PLIO_35_FROM_AIE (70),
        PLIO_36_FROM_AIE (71)
    end
end
