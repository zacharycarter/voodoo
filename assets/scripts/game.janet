(def state @{:camera nil})

(defn init []
  (set (state :camera) (cam/new @{:min-dist 2.0
                                  :max-dist 40.0
                                  :center @[0.0 1.1 0.0]
                                  :distance 3.0
                                  :latitude 20.0
                                  :longitude 20.0})))

(defn event [e]
  (cam/handle-event (state :camera) e))

(defn update []
  (cam/update (state :camera)))

(defn shutdown [])

(defn voodoo []
  @{:width 100
    :height 75
    :init init
    :event event
    :update update
    :shutdown shutdown})
