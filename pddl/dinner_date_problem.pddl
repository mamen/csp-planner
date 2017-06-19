(define (problem dinner-date)
(:domain dinner-date)
(:requirements :strips)
(:init (garbage) (cleanHands) (quiet))
(:goal (and (dinner) (present) (not (garbage))))
)

