(def state @{:camera nil
             :doll-asset: nil
             :player @{:doll nil}})

(defn init []
  (set (state :doll-asset) (asset/load "doll" "/assets/dolls/ozz_skin.doll"))
  (set (state :camera) (cam/orbit @{:min-dist 1.0
                                  :max-dist 50.0
                                  :center @[0.0 0.0 0.0]
                                  :distance 5.0
                                  :latitude 45.0
                                  :longitude 45.0
                                  :nearz 0.01
                                  :farz 2000.0}))

  (v3d/cube @[0.0 5.0 0.0] @[1.0 1.0 1.0])
  (v3d/cube @[0.0 -0.9375 0.0] @[5.0 0.125 5.0])
  (set ((state :player) :doll) (v3d/doll (state :doll-asset))))

(defn event [e]
  (cam/handle-event (state :camera) e))

(defn update []
  (cam/update (state :camera))
  (dbg/draw/camera (state :camera))
  (dbg/draw/grid 0)
  (v3d/draw (state :camera)))

(defn shutdown [])

(defn voodoo []
  @{:width 960
    :height 540
    :init init
    :event event
    :update update
    :shutdown shutdown})
