;; Auto-generated. Do not edit!


(when (boundp 'ivsensorgps::gpsmsg)
  (if (not (find-package "IVSENSORGPS"))
    (make-package "IVSENSORGPS"))
  (shadow 'gpsmsg (find-package "IVSENSORGPS")))
(unless (find-package "IVSENSORGPS::GPSMSG")
  (make-package "IVSENSORGPS::GPSMSG"))

(in-package "ROS")
;;//! \htmlinclude gpsmsg.msg.html


(defclass ivsensorgps::gpsmsg
  :super ros::object
  :slots (_lon _lat _mode _heading _velocity _status _satenum ))

(defmethod ivsensorgps::gpsmsg
  (:init
   (&key
    ((:lon __lon) 0.0)
    ((:lat __lat) 0.0)
    ((:mode __mode) 0)
    ((:heading __heading) 0.0)
    ((:velocity __velocity) 0.0)
    ((:status __status) 0)
    ((:satenum __satenum) 0)
    )
   (send-super :init)
   (setq _lon (float __lon))
   (setq _lat (float __lat))
   (setq _mode (round __mode))
   (setq _heading (float __heading))
   (setq _velocity (float __velocity))
   (setq _status (round __status))
   (setq _satenum (round __satenum))
   self)
  (:lon
   (&optional __lon)
   (if __lon (setq _lon __lon)) _lon)
  (:lat
   (&optional __lat)
   (if __lat (setq _lat __lat)) _lat)
  (:mode
   (&optional __mode)
   (if __mode (setq _mode __mode)) _mode)
  (:heading
   (&optional __heading)
   (if __heading (setq _heading __heading)) _heading)
  (:velocity
   (&optional __velocity)
   (if __velocity (setq _velocity __velocity)) _velocity)
  (:status
   (&optional __status)
   (if __status (setq _status __status)) _status)
  (:satenum
   (&optional __satenum)
   (if __satenum (setq _satenum __satenum)) _satenum)
  (:serialization-length
   ()
   (+
    ;; float64 _lon
    8
    ;; float64 _lat
    8
    ;; byte _mode
    1
    ;; float64 _heading
    8
    ;; float64 _velocity
    8
    ;; byte _status
    1
    ;; byte _satenum
    1
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; float64 _lon
       (sys::poke _lon (send s :buffer) (send s :count) :double) (incf (stream-count s) 8)
     ;; float64 _lat
       (sys::poke _lat (send s :buffer) (send s :count) :double) (incf (stream-count s) 8)
     ;; byte _mode
       (write-byte _mode s)
     ;; float64 _heading
       (sys::poke _heading (send s :buffer) (send s :count) :double) (incf (stream-count s) 8)
     ;; float64 _velocity
       (sys::poke _velocity (send s :buffer) (send s :count) :double) (incf (stream-count s) 8)
     ;; byte _status
       (write-byte _status s)
     ;; byte _satenum
       (write-byte _satenum s)
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; float64 _lon
     (setq _lon (sys::peek buf ptr- :double)) (incf ptr- 8)
   ;; float64 _lat
     (setq _lat (sys::peek buf ptr- :double)) (incf ptr- 8)
   ;; byte _mode
     (setq _mode (sys::peek buf ptr- :char)) (incf ptr- 1)
     (if (> _mode 127) (setq _mode (- _mode 256)))
   ;; float64 _heading
     (setq _heading (sys::peek buf ptr- :double)) (incf ptr- 8)
   ;; float64 _velocity
     (setq _velocity (sys::peek buf ptr- :double)) (incf ptr- 8)
   ;; byte _status
     (setq _status (sys::peek buf ptr- :char)) (incf ptr- 1)
     (if (> _status 127) (setq _status (- _status 256)))
   ;; byte _satenum
     (setq _satenum (sys::peek buf ptr- :char)) (incf ptr- 1)
     (if (> _satenum 127) (setq _satenum (- _satenum 256)))
   ;;
   self)
  )

(setf (get ivsensorgps::gpsmsg :md5sum-) "48b42c3fc827614869a880f65469156f")
(setf (get ivsensorgps::gpsmsg :datatype-) "ivsensorgps/gpsmsg")
(setf (get ivsensorgps::gpsmsg :definition-)
      "float64 lon
float64 lat
byte    mode
float64 heading
float64 velocity
byte    status
byte    satenum

")



(provide :ivsensorgps/gpsmsg "48b42c3fc827614869a880f65469156f")


