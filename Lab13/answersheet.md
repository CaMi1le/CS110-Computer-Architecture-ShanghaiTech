# Lab13

## Ex. 1

No page hits. Because the number of physical memory page is equal to the size of TLB, thus every missing virtual page number in TLB is also missing in PT.

## Ex. 2
00
20
40
60
80
A0
C0
E0
00
20

## Ex.3
Memory Size

Number of physical memory page increased to 8.

### Step 1-4

#### TLB
| VPN | PPN |
| ------ | ------ |
| 0	| 0 |
| 1 | 1 |
| 2 | 2 |
| 3 | 3 |

#### PT
| VPN | PPN |
| ------ | ------ |
| 0	| 0 |
| 1 | 1 |
| 2 | 2 |
| 3 | 3 |
| 4 | - |
| 5 | - |
| 6 | - |
| 7 | - |

### Step 5-8

#### TLB
| VPN | PPN |
| ------ | ------ |
| 4	| 4 |
| 5 | 5 |
| 6 | 6 |
| 7 | 7 |

#### PT
| VPN | PPN |
| ------ | ------ |
| 0	| 0 |
| 1 | 1 |
| 2 | 2 |
| 3 | 3 |
| 4 | 4 |
| 5 | 5 |
| 6 | 6 |
| 7 | 7 |

### Step 9-10

#### TLB
| VPN | PPN |
| ------ | ------ |
| 0	| 0 |
| 1 | 1 |
| 6 | 6 |
| 7 | 7 |

#### PT
| VPN | PPN | Hit |
| ------ | ------ | ------ |
| 0	| 0 | Hit |
| 1 | 1 | Hit |
| 2 | 2 |
| 3 | 3 |
| 4 | 4 |
| 5 | 5 |
| 6 | 6 |
| 7 | 7 |
