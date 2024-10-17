(def actions {:up 0
              :down 1
              :left 2
              :right 3})

(def bindings @{(actions :up) [input/key/w input/gamepad/dpad/up]
                (actions :down) [input/key/s input/gamepad/dpad/down]
                (actions :left) [input/key/a input/gamepad/dpad/left]
                (actions :right) [input/key/d input/gamepad/dpad/right]})

(def player-max-speed 1.0)
(def player-accel 0.2)

(def state @{:blueprint-asset nil
             :blueprint-img nil
             :camera nil
             :doll-asset nil
             :player nil
             :player-velocity 0.0
             :player-angular-velocity 0.0})

(defn gen []
  (def {:w w
        :h h
        :n n
        :pixels pixels} (state :blueprint-img))
  (prin "\n")
  (for y 0 h
    (for x 0 w
      (do
        (def r (get pixels (* (+ x (* w y)) 4)))
        (def g (get pixels (+ 1 (* (+ x (* w y)) 4))))
        (def b (get pixels (+ 2 (* (+ x (* w y)) 4))))
        (def a (get pixels (+ 3 (* (+ x (* w y)) 4))))
        (if (and (= a 255) (and (= b 0) (and (= g 0) (= r 0)))) (prin ".") (prin " "))))
    (prin "\n")))

  # (def len (length pixels))
  # (var idx 0)
  # (var row 0)
  # (var col 0)
  # (while (< (- (+ idx n) 1) len)
  #   (printf "R: %d, G: %d, B: %d, A: %d"
  #           (get pixels idx)
  #           (get pixels (+ idx 1))
  #           (get pixels (+ idx 2))
  #           (get pixels (+ idx 3)))
  #   (++ col)
  #   (if (= col w) (do (set col 0) (++ row)))
  #   (+= idx n)))

      # (if (> (get pixels (+ (* w row) col)) 0) (prinf "%s" "+") (prinf "%s" " ")))

(defn init []
  (set (state :blueprint-asset) (asset/load "image" "assets/ship_blueprints/battleship_00.png" @{}))
  (set (state :doll-asset) (asset/load "doll" "assets/dolls/character.doll"))
  (set (state :blueprint-img) (gfx/img/get (state :blueprint-asset)))

  (gen)

  (set (state :camera) (game/object @{:x 0.0 :y 0.0 :z 0.0} @{:x 0.0 :y 0.0 :z 0.0} @{:x 0.0 :y 0.0 :z 0.0 :w 1.0}))
  (game/object/set (state :camera) component/camera (cam/orbit @{:min-dist 1.0
                                                                 :max-dist 50.0
                                                                 :center @{:x 0 :y 0 :z 0}
                                                                 :distance 5.0
                                                                 :latitude 45.0
                                                                 :longitude 0.0
                                                                 :nearz 0.01
                                                                 :farz 2000.0}))

  (v3d/cube @[0.0 5.0 0.0] @[1.0 1.0 1.0])
  (v3d/cube @[0.0 -0.9375 0.0] @[5.0 0.125 5.0])
  (set (state :player) (game/object @{:x 0.0 :y 0.0 :z 0.0} @{:x 1.0 :y 1.0 :z 1.0} @{:x 0.0 :y 0.0 :z 0.0 :w 1.0}))
  (game/object/set (state :player) component/doll (v3d/doll/create (state :doll-asset)))
  (loop [[action binding] :pairs bindings]
    (if (not= (get binding 0) input/invalid)
      (input/bind input/layer/user (get binding 0) action))
    (if (not= (get binding 1) input/invalid)
      (input/bind input/layer/user (get binding 1) action))))

(defn event [e])

(defn update-player [dt cam]
  (var {:position player-pos
        :rotation player-rot} (game/object/get (state :player) component/transform))
  (def {:eye_pos cam-pos
        :eye_dir cam-dir} cam)
  (def doll (game/object/get (state :player) component/doll))
  # (def delta (* 30 dt))
  # (def speed (* 0.2 delta))

  # (if (not= (input/state (actions :up)) 0)
  #   (do
  #     (set (state :player-velocity) (+ (state :player-velocity) player-accel))
  #     (if (> (state :player-velocity) player-max-speed)
  #       (set (state :player-velocity) player-max-speed)))
  #   (if (not= (input/state (actions :down)) 0)
  #     (do
  #       (set (state :player-velocity) (- (state :player-velocity) player-accel))
  #       (if (< (state :player-velocity) (- player-max-speed))
  #         (set (state :player-velocity) (- player-max-speed))))
  #     (if (< (state :player-velocity) 0)
  #       (do
  #         (set (state :player-velocity) (+ (state :player-velocity) player-accel))
  #         (if (>= (state :player-velocity) 0)
  #           (set (state :player-velocity) 0)))
  #       (do
  #         (set (state :player-velocity) (- (state :player-velocity) player-accel))
  #         (if (<= (state :player-velocity) 0)
  #           (set (state :player-velocity) 0))))))

  # (cond
  #   (> (input/state (actions :right)) 0) (do
  #                                          (set (state :player-angular-velocity) (+ (state :player-angular-velocity) player-accel))
  #                                          (if (> (state :player-angular-velocity) player-max-speed)
  #                                            (set (state :player-angular-velocity) player-max-speed)))
  #   (> (input/state (actions :left)) 0) (do
  #                                         (set (state :player-angular-velocity) (- (state :player-angular-velocity) player-accel))
  #                                         (if (< (state :player-angular-velocity) (- player-max-speed))
  #                                           (set (state :player-angular-velocity) (- player-max-speed))))
  #   (if (< (state :player-angularvelocity) 0)
  #     (do
  #       (set (state :player-angular-velocity) (+ (state :player-angular-velocity) player-accel))
  #       (if (>= (state :player-angular-velocity) 0)
  #         (set (state :player-angular-velocity) 0)))
  #     (do
  #       (set (state :player-angular-velocity) (- (state :player-angular-velocity) player-accel))
  #       (if (<= (state :player-angular-velocity) 0)
  #         (set (state :player-angular-velocity) 0)))))

  # (def rot (math/mul-q (math/axis-angle-q @{:x 0 :y 1 :z 0} (math/to-rad (* (* speed (state :player-angular-velocity)) 20))) player-rot))
  # (def pos
  #   (math/add-v3
  #     @{:x player-x :y player-y :z player-z}
  #     (math/mul-v3f
  #     (math/rotate-v3q
  #       @{:x 0 :y 0 :z 1}
  #       rot)
  #     (* speed (state :player-velocity)))))

  # (game/object/set (state :player) component/doll
  #                   @{:blend_ratio (math/abs (state :player-velocity))})

  # (game/object/set (state :player) component/transform
  #                   @{:position pos
  #                     :rotation rot})

  # (set (cam :center) pos)
  # (game/object/set (state :camera) component/camera cam))

    (def delta (* 30 dt))
    (def speed (* 0.2 delta))

    (def axis-x (- (input/state (actions :right)) (input/state (actions :left))))
    (def axis-y (- (input/state (actions :up)) (input/state (actions :down))))
    (def cam-fwd (math/norm-v3 (math/sub-v3 player-pos cam-pos)))
    (def cam-right (math/norm-v3 (math/cross cam-fwd @{:x 0 :y 1 :z 0})))
    (def player-dir
      (put
       (math/norm-v3
        (math/add-v3
         (math/mul-v3f cam-fwd axis-y)
         (math/mul-v3f cam-right axis-x)))
       :y 0))

    # (pp player-rot)
    (def player-fwd (math/rotate-v3q @{:x 0 :y (- 1) :z 0} player-rot))
    # (print (math/to-deg (- (math/atan2 (:z player-fwd) (:x player-fwd)) (math/atan2 (:z player-dir) (:x player-dir)))))

    (game/object/set
     (state :player)
     component/transform
     @{:position (math/add-v3 player-pos (math/mul-v3f player-dir (* speed 2)))}))

(defn update [dt]
  (var camera (game/object/get (state :camera) component/camera))

  (update-player dt camera)
  (game/object/set (state :camera) component/camera (cam/update camera))
  (dbg/draw/camera camera)
  (dbg/draw/grid 0)
  (v3d/draw camera))

(defn shutdown [])

(defn voodoo []
  (setdyn :pretty-format "%m")
  @{:width 960
    :height 540
    :init init
    :event event
    :update update
    :shutdown shutdown})
