(define (domain dinner-date)
	(:requirements :strips)

	(:predicates (garbage) (cleanHands) (quiet) (dinner) (present))

        (:action cook
         :parameters ()
         :precondition (cleanHands)
         :effect (dinner)
        )

	(:action wrap
	:parameters ()
	:precondition (quiet)
	:effect (present)
	)

	(:action carry
 	:parameters ()
        :precondition ()
	:effect (and (not (garbage)) (not(cleanHands)))
	)

	(:action dolly
        :parameters ()
        :precondition ()
	:effect (and (not (garbage)) (not(quiet)))
	)

)
