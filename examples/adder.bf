,           ; read to p1
>           ; move to p2
,           ; read to p2
[           ; enter loop
    <       ; move to p1
    +       ; increment p1
    >       ; move to p2
    -       ; decrement p2
]           ; exit when last cell is empty
<           ; go back to p1
------------------------------------------------ ; subtract 48 
.           ; print p1
