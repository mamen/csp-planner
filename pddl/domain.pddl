(define (domain escape)
    (:requirements :strips)
    (:predicates (infirmary_code)
                (controlroom_code)
                (knife)
                (exit_locked)
                (key)
                (axe)
                (motor)
                (at_bunker)
                (at_infirmary)
                (at_corridor)
                (at_armory)
                (at_controlroom)
                (at_street)
                (at_woods)
                (at_lake)
                (at_cabin)
                (at_cellar)
                (at_city)
    )
    
    (:action find_infirmarycode
        :parameters ()
        :precondition (and
            (at_infirmary)
            (not (infirmary_code))
        )
        :effect (and
            (infirmary_code)
        )
    )
    
    (:action goto_corridor
        :parameters ()
        :precondition (or
            (and
                (at_infirmary)
                (infirmary_code)
            )
            (at_armory)
            (at_controlroom)
        )
        :effect(and
            (at_corridor)
            (not (at_infirmary))
            (not (at_armory))
            (not (at_controlroom))
        )
    )
    
    (:action goto_armory
        :parameters ()
        :precondition (and
            (at_corridor)
        )
        :effect (and 
            (at_armory)
            (not (at_corridor))
        )
    )
    
    (:action find_knife
        :parameters ()
        :precondition (and 
            (at_armory)
        )
        :effect(and
            (knife)
        )
    )
    
    (:action open_cuppord
        :parameters ()
        :precondition(and
            (at_armory)
            (knife)
        )
        :effect(and
            (controlroom_code)
            (not (knife))
        )
    )
    
    (:action goto_controlroom
        :parameters ()
        :precondition(and
            (at_corridor)
            (controlroom_code)
        )
        :effect (and
            (at_controlroom)
            (not (at_corridor))
        )
    )
    
    (:action unlock_exit
        :parameters ()
        :precondition(and
            (exit_locked)
            (at_controlroom)
        )
        :effect(and
            (not (exit_locked))
        )
    )
    
    (:action leave_bunker
        :parameters ()
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
        :parameters ()
        :precondition (and
            (at_bunker)
        )
        :effect(and
            (at_street)
            (not (at_bunker))
        )
    )
    
    (:action goto_bunker
        :parameters ()
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
        :parameters ()
        :precondition(or
            (at_bunker)
            (at_cabin)
            (at_cellar)
            (at_lake)
        )
        :effect(and
            (at_woods)
            (not (at_bunker))
            (not (at_cabin))
            (not (at_cellar))
            (not (at_lake))
        )
    )
    
    (:action goto_cabin
        :parameters ()
        :precondition(and
            (at_woods)
        )
        :effect(and
            (at_cabin)
            (not (at_woods))
        )
    )
    
    (:action goto_cellar
        :parameters ()
        :precondition(and
            (at_woods)
            (key)
        )
        :effect(and
            (at_cellar)
            (not (at_woods))
        )
    )
    
    (:action goto_lake
        :parameters ()
        :precondition(and
            (at_woods)
        )
        :effect(and
            (at_lake)
            (not (at_woods))
        )
    )
    
    (:action find_key
        :parameters ()
        :precondition(and
            (at_cabin)
        )
        :effect(and
            (key)
        )
    )
    
    (:action find_axe
        :parameters ()
        :precondition(and
            (at_cellar)
        )
        :effect(and
            (axe)
        )
    )
    
    (:action open_box
        :parameters ()
        :precondition(and
            (axe)
            (at_cellar)
        )
        :effect(and
            (motor)
        )
    )
    
    (:action goto_city
        :parameters ()
        :precondition(and
            (at_lake)
            (motor)
        )
        :effect(and
            (at_city)
            (not (motor))
            (not (at_lake))
        )
    )
    
)