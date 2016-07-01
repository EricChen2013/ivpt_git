
(cl:in-package :asdf)

(defsystem "ivsensorgps-msg"
  :depends-on (:roslisp-msg-protocol :roslisp-utils )
  :components ((:file "_package")
    (:file "gpsmsg" :depends-on ("_package_gpsmsg"))
    (:file "_package_gpsmsg" :depends-on ("_package"))
  ))