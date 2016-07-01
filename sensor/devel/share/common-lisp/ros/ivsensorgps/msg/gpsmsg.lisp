; Auto-generated. Do not edit!


(cl:in-package ivsensorgps-msg)


;//! \htmlinclude gpsmsg.msg.html

(cl:defclass <gpsmsg> (roslisp-msg-protocol:ros-message)
  ((lon
    :reader lon
    :initarg :lon
    :type cl:float
    :initform 0.0)
   (lat
    :reader lat
    :initarg :lat
    :type cl:float
    :initform 0.0)
   (mode
    :reader mode
    :initarg :mode
    :type cl:integer
    :initform 0)
   (heading
    :reader heading
    :initarg :heading
    :type cl:float
    :initform 0.0)
   (velocity
    :reader velocity
    :initarg :velocity
    :type cl:float
    :initform 0.0)
   (status
    :reader status
    :initarg :status
    :type cl:integer
    :initform 0)
   (satenum
    :reader satenum
    :initarg :satenum
    :type cl:integer
    :initform 0))
)

(cl:defclass gpsmsg (<gpsmsg>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <gpsmsg>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'gpsmsg)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name ivsensorgps-msg:<gpsmsg> is deprecated: use ivsensorgps-msg:gpsmsg instead.")))

(cl:ensure-generic-function 'lon-val :lambda-list '(m))
(cl:defmethod lon-val ((m <gpsmsg>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader ivsensorgps-msg:lon-val is deprecated.  Use ivsensorgps-msg:lon instead.")
  (lon m))

(cl:ensure-generic-function 'lat-val :lambda-list '(m))
(cl:defmethod lat-val ((m <gpsmsg>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader ivsensorgps-msg:lat-val is deprecated.  Use ivsensorgps-msg:lat instead.")
  (lat m))

(cl:ensure-generic-function 'mode-val :lambda-list '(m))
(cl:defmethod mode-val ((m <gpsmsg>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader ivsensorgps-msg:mode-val is deprecated.  Use ivsensorgps-msg:mode instead.")
  (mode m))

(cl:ensure-generic-function 'heading-val :lambda-list '(m))
(cl:defmethod heading-val ((m <gpsmsg>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader ivsensorgps-msg:heading-val is deprecated.  Use ivsensorgps-msg:heading instead.")
  (heading m))

(cl:ensure-generic-function 'velocity-val :lambda-list '(m))
(cl:defmethod velocity-val ((m <gpsmsg>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader ivsensorgps-msg:velocity-val is deprecated.  Use ivsensorgps-msg:velocity instead.")
  (velocity m))

(cl:ensure-generic-function 'status-val :lambda-list '(m))
(cl:defmethod status-val ((m <gpsmsg>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader ivsensorgps-msg:status-val is deprecated.  Use ivsensorgps-msg:status instead.")
  (status m))

(cl:ensure-generic-function 'satenum-val :lambda-list '(m))
(cl:defmethod satenum-val ((m <gpsmsg>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader ivsensorgps-msg:satenum-val is deprecated.  Use ivsensorgps-msg:satenum instead.")
  (satenum m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <gpsmsg>) ostream)
  "Serializes a message object of type '<gpsmsg>"
  (cl:let ((bits (roslisp-utils:encode-double-float-bits (cl:slot-value msg 'lon))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 32) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 40) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 48) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 56) bits) ostream))
  (cl:let ((bits (roslisp-utils:encode-double-float-bits (cl:slot-value msg 'lat))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 32) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 40) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 48) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 56) bits) ostream))
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'mode)) ostream)
  (cl:let ((bits (roslisp-utils:encode-double-float-bits (cl:slot-value msg 'heading))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 32) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 40) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 48) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 56) bits) ostream))
  (cl:let ((bits (roslisp-utils:encode-double-float-bits (cl:slot-value msg 'velocity))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 32) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 40) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 48) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 56) bits) ostream))
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'status)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'satenum)) ostream)
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <gpsmsg>) istream)
  "Deserializes a message object of type '<gpsmsg>"
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 32) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 40) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 48) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 56) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'lon) (roslisp-utils:decode-double-float-bits bits)))
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 32) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 40) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 48) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 56) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'lat) (roslisp-utils:decode-double-float-bits bits)))
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'mode)) (cl:read-byte istream))
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 32) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 40) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 48) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 56) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'heading) (roslisp-utils:decode-double-float-bits bits)))
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 32) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 40) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 48) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 56) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'velocity) (roslisp-utils:decode-double-float-bits bits)))
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'status)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'satenum)) (cl:read-byte istream))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<gpsmsg>)))
  "Returns string type for a message object of type '<gpsmsg>"
  "ivsensorgps/gpsmsg")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'gpsmsg)))
  "Returns string type for a message object of type 'gpsmsg"
  "ivsensorgps/gpsmsg")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<gpsmsg>)))
  "Returns md5sum for a message object of type '<gpsmsg>"
  "48b42c3fc827614869a880f65469156f")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'gpsmsg)))
  "Returns md5sum for a message object of type 'gpsmsg"
  "48b42c3fc827614869a880f65469156f")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<gpsmsg>)))
  "Returns full string definition for message of type '<gpsmsg>"
  (cl:format cl:nil "float64 lon~%float64 lat~%byte    mode~%float64 heading~%float64 velocity~%byte    status~%byte    satenum~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'gpsmsg)))
  "Returns full string definition for message of type 'gpsmsg"
  (cl:format cl:nil "float64 lon~%float64 lat~%byte    mode~%float64 heading~%float64 velocity~%byte    status~%byte    satenum~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <gpsmsg>))
  (cl:+ 0
     8
     8
     1
     8
     8
     1
     1
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <gpsmsg>))
  "Converts a ROS message object to a list"
  (cl:list 'gpsmsg
    (cl:cons ':lon (lon msg))
    (cl:cons ':lat (lat msg))
    (cl:cons ':mode (mode msg))
    (cl:cons ':heading (heading msg))
    (cl:cons ':velocity (velocity msg))
    (cl:cons ':status (status msg))
    (cl:cons ':satenum (satenum msg))
))
