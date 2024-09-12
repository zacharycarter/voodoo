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

(def state @{:camera nil
             :doll-asset: nil
             :player: nil
             :player-velocity 0.0
             :player-angular-velocity 0.0})

(defn init []
  (set (state :doll-asset) (asset/load "doll" "assets/dolls/character.doll"))

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
  (var {:position {:x player-x
                   :y player-y
                   :z player-z}
        :rotation player-rot} (game/object/get (state :player) component/transform))
  (def {:eye_pos
        {:x cam-x :y cam-y :z cam-z}
        :eye_dir
        {:x cam-dir-x :y cam-dir-y :z cam-dir-z}} cam)
  (def doll (game/object/get (state :player) component/doll))
  (def delta (* 30 dt))
  (def speed (* 0.2 delta))

  (if (not= (input/state (actions :up)) 0)
    (do
      (set (state :player-velocity) (+ (state :player-velocity) player-accel))
      (if (> (state :player-velocity) player-max-speed)
        (set (state :player-velocity) player-max-speed)))
    (if (not= (input/state (actions :down)) 0)
      (do
        (set (state :player-velocity) (- (state :player-velocity) player-accel))
        (if (< (state :player-velocity) (- player-max-speed))
          (set (state :player-velocity) (- player-max-speed))))
      (if (< (state :player-velocity) 0)
        (do
          (set (state :player-velocity) (+ (state :player-velocity) player-accel))
          (if (>= (state :player-velocity) 0)
            (set (state :player-velocity) 0)))
        (do
          (set (state :player-velocity) (- (state :player-velocity) player-accel))
          (if (<= (state :player-velocity) 0)
            (set (state :player-velocity) 0))))))

  (cond
    (> (input/state (actions :right)) 0) (do
                                           (set (state :player-angular-velocity) (+ (state :player-angular-velocity) player-accel))
                                           (if (> (state :player-angular-velocity) player-max-speed)
                                             (set (state :player-angular-velocity) player-max-speed)))
    (> (input/state (actions :left)) 0) (do
                                          (set (state :player-angular-velocity) (- (state :player-angular-velocity) player-accel))
                                          (if (< (state :player-angular-velocity) (- player-max-speed))
                                            (set (state :player-angular-velocity) (- player-max-speed))))
    (if (< (state :player-angularvelocity) 0)
      (do
        (set (state :player-angular-velocity) (+ (state :player-angular-velocity) player-accel))
        (if (>= (state :player-angular-velocity) 0)
          (set (state :player-angular-velocity) 0)))
      (do
        (set (state :player-angular-velocity) (- (state :player-angular-velocity) player-accel))
        (if (<= (state :player-angular-velocity) 0)
          (set (state :player-angular-velocity) 0)))))

  (def rot (math/mul-q (math/axis-angle-q @{:x 0 :y 1 :z 0} (math/to-rad (* (* speed (state :player-angular-velocity)) 20))) player-rot))
  (def pos
    (math/add-v3
      @{:x player-x :y player-y :z player-z}
      (math/mul-v3f
      (math/rotate-v3q
        @{:x 0 :y 0 :z 1}
        rot)
      (* speed (state :player-velocity)))))

  (game/object/set (state :player) component/doll
                    @{:blend_ratio (math/abs (state :player-velocity))})

  (game/object/set (state :player) component/transform
                    @{:position pos
                      :rotation rot})

  (set (cam :center) pos)
  (game/object/set (state :camera) component/camera cam))

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
