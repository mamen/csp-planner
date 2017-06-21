(define (domain escape)
   (:requirements :strips)
   (:predicates (infirmary_code)
                (controllroom_code)
                (knife)
                (exit_locked)
                (key)
                (axe)
                (motor)
                (city)
                (find_infirmarycode)
                (find_knife)
                (find_key)
                (find_axe)
                (open_box)
                (open_cuppord)
                (unlock_exit)
                (at_bunker)
                (at_infirmary)
                (at_corridor)
                (at_armory)
                (at_controllroom)
                (at_street)
                (at_woods)
                (at_lake)
                (at_cabin)
                (at_celler)
                (leave_bunker)
                (goto_corridor)
                (goto_armory)
                (goto_controllroom)
                (goto_street)
                (goto_bunker)
                (goto_woods)
                (goto_lake)
                (goto_cabin)
                (goto_celler)
                (goto_city)
    )
    
    (:action find_infirmarycode
        :precondition (and
            (at_infirmary)
            (not (infirmary_code))
        )
        :effect (and
            (infirmary_code)
        )
    )
    
    (:action goto_corridor
        :precondition (or
            (and
                (at_infirmary)
                (infirmary_code)
            )
            (at_armory)
            (at_controllroom)
        )
        :effect(and
            (at_corridor)
            (not (at_infirmary))
            (not (at_armory))
            (not (at_controllroom))
        )
    )
    
    (:action goto_armory
        :precondition (and
            (at_corridor)
        )
        :effect (and 
            (at_armory)
            (not (at_corridor))
        )
    )
    
    (:action find_knife
        :precondition (and 
            (at_armory)
        )
        :effect(and
            (knife)
        )
    )
    
    (:action open_cuppord
        :precondition(and
            (at_armory)
            (knife)
        )
        :effect(and
            (controllroom_code)
            (not (knife))
        )
    )
    
    (:action goto_controllroom
        :precondition(and
            (at_corridor)
            (controllroom_code)
        )
        :effect (and
            (at_controllroom)
            (not (at_corridor))
        )
    )
    
    (:action unlock_exit
        :precondition(and
            (exit_locked)
            (at_controllroom)
        )
        :effect(and
            (not (exit_locked))
        )
    )
    
    (:action leave_bunker
        :precondition(and 
            (at_corridor)
            (not (exit_locked))
        )
        :effect (and 
            (not (at_corridor))
            (at_bunker)
        )
    )
    
    (:action goto_street
        :precondition (and
            (at_bunker)
        )
        :effect(and
            (at_street)
            (not (at_bunker))
        )
    )
    
    (:action goto_bunker
        :precondition(or 
            (at_street)
            (at_woods)
        )
        :effect(and
            (at_bunker)
            (not (at_street))
            (not (at_woods))
        )
    )
    
    (:action goto_woods
        :precondition(or
            (at_bunker)
            (at_cabin)
            (at_celler)
            (at_lake)
        )
        :effect(and
            (at_woods)
            (not (at_bunker))
            (not (at_cabin))
            (not (at_celler))
            (not (at_lake))
        )
    )
    
    (:action goto_cabin
        :precondition(and 
            (at_woods)
        )
        :effect(and
            (at_cabin)
            (not (at_woods))
        )
    )
    
    (:action goto_celler
        :precondition(and
            (at_woods)
            (key)
        )
        :effect(and
            (at_celler)
            (not (at_woods))
        )
    )
    
    (:action goto_lake
        :precondition(and
            (at_woods)
        )
        :effect(and
            (at_lake)
            (not (at_woods))
        )
    )
    
    (:action find_key
        :precondition(and
            (at_cabin)
        )
        :effect(and
            (key)
        )
    )
    
    (:action find_axe
        :precondition(and
            (at_celler)
        )
        :effect(and
            (axe)
        )
    )
    
    (:action open_box
        :precondition(and
            (axe)
            (at_celler)
        )
        :effect(and
            (motor)
        )
    )
    
    (:action goto_city
        :precondition(and
            (at_lake)
            (motor)
        )
        :effect(and
            (city)
            (not (motor))
            (not (at_lake))
        )
    )
    
)