//
// basic_socket_ext.hpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
// Copyright (C) 2018 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_ASIO_BASIC_SOCKET_EXT_HPP
#define BOOST_ASIO_BASIC_SOCKET_EXT_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <boost/asio/detail/config.hpp>
#include <boost/asio/async_result.hpp>
#include <boost/asio/basic_io_object.hpp>
#include <boost/asio/detail/handler_type_requirements.hpp>
#include <boost/asio/detail/throw_error.hpp>
#include <boost/asio/detail/type_traits.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/post.hpp>
#include <boost/asio/socket_base.hpp>

#if defined(BOOST_ASIO_HAS_MOVE)
# include <utility>
#endif // defined(BOOST_ASIO_HAS_MOVE)

#if !defined(BOOST_ASIO_ENABLE_OLD_SERVICES)
# if defined(BOOST_ASIO_WINDOWS_RUNTIME)
#  include <boost/asio/detail/winrt_ssocket_service.hpp>
#  define BOOST_ASIO_SVC_T detail::winrt_ssocket_service<Protocol>
# elif defined(BOOST_ASIO_HAS_IOCP)
#  include <boost/asio/detail/win_iocp_socket_service.hpp>
#  define BOOST_ASIO_SVC_T detail::win_iocp_socket_service<Protocol>
# else
#  include <boost/asio/detail/reactive_socket_service_ext.hpp>
#  define BOOST_ASIO_SVC_T detail::reactive_socket_service_ext<Protocol>
# endif
#endif // !defined(BOOST_ASIO_ENABLE_OLD_SERVICES)

#include <boost/asio/detail/push_options.hpp>

namespace boost {
namespace asio {

/// Provides socket functionality.
/**
 * The basic_socket class template provides functionality that is common to both
 * stream-oriented and datagram-oriented sockets.
 *
 * @par Thread Safety
 * @e Distinct @e objects: Safe.@n
 * @e Shared @e objects: Unsafe.
 */
template <typename Protocol BOOST_ASIO_SVC_TPARAM>
class basic_socket_ext
  : BOOST_ASIO_SVC_ACCESS basic_io_object<BOOST_ASIO_SVC_T>,
    public socket_base
{
public:
  /// The type of the executor associated with the object.
  typedef io_context::executor_type executor_type;

  /// The native representation of a socket.
#if defined(GENERATING_DOCUMENTATION)
  typedef implementation_defined native_handle_type;
#else
  typedef typename BOOST_ASIO_SVC_T::native_handle_type native_handle_type;
#endif

  /// The protocol type.
  typedef Protocol protocol_type;

  /// The endpoint type.
  typedef typename Protocol::endpoint endpoint_type;

#if !defined(BOOST_ASIO_NO_EXTENSIONS)
  /// A basic_socket is always the lowest layer.
  typedef basic_socket_ext<Protocol BOOST_ASIO_SVC_TARG> lowest_layer_type;
#endif // !defined(BOOST_ASIO_NO_EXTENSIONS)

  /// Construct a basic_socket without opening it.
  /**
   * This constructor creates a socket without opening it.
   *
   * @param io_context The io_context object that the socket will use to
   * dispatch handlers for any asynchronous operations performed on the socket.
   */
  explicit basic_socket_ext(boost::asio::io_context& io_context)
    : basic_io_object<BOOST_ASIO_SVC_T>(io_context)
  {
  }

  /// Construct and open a basic_socket.
  /**
   * This constructor creates and opens a socket.
   *
   * @param io_context The io_context object that the socket will use to
   * dispatch handlers for any asynchronous operations performed on the socket.
   *
   * @param protocol An object specifying protocol parameters to be used.
   *
   * @throws boost::system::system_error Thrown on failure.
   */
  basic_socket_ext(boost::asio::io_context& io_context,
      const protocol_type& protocol)
    : basic_io_object<BOOST_ASIO_SVC_T>(io_context)
  {
    boost::system::error_code ec;
    this->get_service().open(this->get_implementation(), protocol, ec);
    boost::asio::detail::throw_error(ec, "open");
  }

  /// Construct a basic_socket, opening it and binding it to the given local
  /// endpoint.
  /**
   * This constructor creates a socket and automatically opens it bound to the
   * specified endpoint on the local machine. The protocol used is the protocol
   * associated with the given endpoint.
   *
   * @param io_context The io_context object that the socket will use to
   * dispatch handlers for any asynchronous operations performed on the socket.
   *
   * @param endpoint An endpoint on the local machine to which the socket will
   * be bound.
   *
   * @throws boost::system::system_error Thrown on failure.
   */
  basic_socket_ext(boost::asio::io_context& io_context,
      const endpoint_type& endpoint)
    : basic_io_object<BOOST_ASIO_SVC_T>(io_context)
  {
    boost::system::error_code ec;
    const protocol_type protocol = endpoint.protocol();
    this->get_service().open(this->get_implementation(), protocol, ec);
    boost::asio::detail::throw_error(ec, "open");
    this->get_service().bind(this->get_implementation(), endpoint, ec);
    boost::asio::detail::throw_error(ec, "bind");
  }

  /// Construct a basic_socket on an existing native socket.
  /**
   * This constructor creates a socket object to hold an existing native socket.
   *
   * @param io_context The io_context object that the socket will use to
   * dispatch handlers for any asynchronous operations performed on the socket.
   *
   * @param protocol An object specifying protocol parameters to be used.
   *
   * @param native_socket A native socket.
   *
   * @throws boost::system::system_error Thrown on failure.
   */
  basic_socket_ext(boost::asio::io_context& io_context,
      const protocol_type& protocol, const native_handle_type& native_socket)
    : basic_io_object<BOOST_ASIO_SVC_T>(io_context)
  {
    boost::system::error_code ec;
    this->get_service().assign(this->get_implementation(),
        protocol, native_socket, ec);
    boost::asio::detail::throw_error(ec, "assign");
  }

#if defined(BOOST_ASIO_HAS_MOVE) || defined(GENERATING_DOCUMENTATION)
  /// Move-construct a basic_socket from another.
  /**
   * This constructor moves a socket from one object to another.
   *
   * @param other The other basic_socket object from which the move will
   * occur.
   *
   * @note Following the move, the moved-from object is in the same state as if
   * constructed using the @c basic_socket(io_context&) constructor.
   */
  basic_socket_ext(basic_socket_ext&& other)
    : basic_io_object<BOOST_ASIO_SVC_T>(std::move(other))
  {
  }

  /// Move-assign a basic_socket from another.
  /**
   * This assignment operator moves a socket from one object to another.
   *
   * @param other The other basic_socket object from which the move will
   * occur.
   *
   * @note Following the move, the moved-from object is in the same state as if
   * constructed using the @c basic_socket(io_context&) constructor.
   */
  basic_socket_ext& operator=(basic_socket_ext&& other)
  {
    basic_io_object<BOOST_ASIO_SVC_T>::operator=(std::move(other));
    return *this;
  }

  // All sockets have access to each other's implementations.
  template <typename Protocol1 BOOST_ASIO_SVC_TPARAM1>
  friend class basic_socket;

  /// Move-construct a basic_socket from a socket of another protocol type.
  /**
   * This constructor moves a socket from one object to another.
   *
   * @param other The other basic_socket object from which the move will
   * occur.
   *
   * @note Following the move, the moved-from object is in the same state as if
   * constructed using the @c basic_socket(io_context&) constructor.
   */
  template <typename Protocol1 BOOST_ASIO_SVC_TPARAM1>
  basic_socket_ext(basic_socket_ext<Protocol1 BOOST_ASIO_SVC_TARG1>&& other,
      typename enable_if<is_convertible<Protocol1, Protocol>::value>::type* = 0)
    : basic_io_object<BOOST_ASIO_SVC_T>(
        other.get_service(), other.get_implementation())
  {
  }

  /// Move-assign a basic_socket from a socket of another protocol type.
  /**
   * This assignment operator moves a socket from one object to another.
   *
   * @param other The other basic_socket object from which the move will
   * occur.
   *
   * @note Following the move, the moved-from object is in the same state as if
   * constructed using the @c basic_socket(io_context&) constructor.
   */
  template <typename Protocol1 BOOST_ASIO_SVC_TPARAM1>
  typename enable_if<is_convertible<Protocol1, Protocol>::value,
      basic_socket_ext>::type& operator=(
        basic_socket_ext<Protocol1 BOOST_ASIO_SVC_TARG1>&& other)
  {
    basic_socket_ext tmp(std::move(other));
    basic_io_object<BOOST_ASIO_SVC_T>::operator=(std::move(tmp));
    return *this;
  }
#endif // defined(BOOST_ASIO_HAS_MOVE) || defined(GENERATING_DOCUMENTATION)

#if defined(BOOST_ASIO_ENABLE_OLD_SERVICES)
  // These functions are provided by basic_io_object<>.
#else // defined(BOOST_ASIO_ENABLE_OLD_SERVICES)
#if !defined(BOOST_ASIO_NO_DEPRECATED)
  /// (Deprecated: Use get_executor().) Get the io_context associated with the
  /// object.
  /**
   * This function may be used to obtain the io_context object that the I/O
   * object uses to dispatch handlers for asynchronous operations.
   *
   * @return A reference to the io_context object that the I/O object will use
   * to dispatch handlers. Ownership is not transferred to the caller.
   */
  boost::asio::io_context& get_io_context()
  {
    return basic_io_object<BOOST_ASIO_SVC_T>::get_io_context();
  }

  /// (Deprecated: Use get_executor().) Get the io_context associated with the
  /// object.
  /**
   * This function may be used to obtain the io_context object that the I/O
   * object uses to dispatch handlers for asynchronous operations.
   *
   * @return A reference to the io_context object that the I/O object will use
   * to dispatch handlers. Ownership is not transferred to the caller.
   */
  boost::asio::io_context& get_io_service()
  {
    return basic_io_object<BOOST_ASIO_SVC_T>::get_io_service();
  }
#endif // !defined(BOOST_ASIO_NO_DEPRECATED)

  /// Get the executor associated with the object.
  executor_type get_executor() BOOST_ASIO_NOEXCEPT
  {
    return basic_io_object<BOOST_ASIO_SVC_T>::get_executor();
  }
#endif // defined(BOOST_ASIO_ENABLE_OLD_SERVICES)

#if !defined(BOOST_ASIO_NO_EXTENSIONS)
  /// Get a reference to the lowest layer.
  /**
   * This function returns a reference to the lowest layer in a stack of
   * layers. Since a basic_socket cannot contain any further layers, it simply
   * returns a reference to itself.
   *
   * @return A reference to the lowest layer in the stack of layers. Ownership
   * is not transferred to the caller.
   */
  lowest_layer_type& lowest_layer()
  {
    return *this;
  }

  /// Get a const reference to the lowest layer.
  /**
   * This function returns a const reference to the lowest layer in a stack of
   * layers. Since a basic_socket cannot contain any further layers, it simply
   * returns a reference to itself.
   *
   * @return A const reference to the lowest layer in the stack of layers.
   * Ownership is not transferred to the caller.
   */
  const lowest_layer_type& lowest_layer() const
  {
    return *this;
  }
#endif // !defined(BOOST_ASIO_NO_EXTENSIONS)

  /// Open the socket using the specified protocol.
  /**
   * This function opens the socket so that it will use the specified protocol.
   *
   * @param protocol An object specifying protocol parameters to be used.
   *
   * @throws boost::system::system_error Thrown on failure.
   *
   * @par Example
   * @code
   * boost::asio::ip::tcp::socket socket(io_context);
   * socket.open(boost::asio::ip::tcp::v4());
   * @endcode
   */
  void open(const protocol_type& protocol = protocol_type())
  {
    boost::system::error_code ec;
    this->get_service().open(this->get_implementation(), protocol, ec);
    boost::asio::detail::throw_error(ec, "open");
  }

  /// Open the socket using the specified protocol.
  /**
   * This function opens the socket so that it will use the specified protocol.
   *
   * @param protocol An object specifying which protocol is to be used.
   *
   * @param ec Set to indicate what error occurred, if any.
   *
   * @par Example
   * @code
   * boost::asio::ip::tcp::socket socket(io_context);
   * boost::system::error_code ec;
   * socket.open(boost::asio::ip::tcp::v4(), ec);
   * if (ec)
   * {
   *   // An error occurred.
   * }
   * @endcode
   */
  BOOST_ASIO_SYNC_OP_VOID open(const protocol_type& protocol,
      boost::system::error_code& ec)
  {
    this->get_service().open(this->get_implementation(), protocol, ec);
    BOOST_ASIO_SYNC_OP_VOID_RETURN(ec);
  }

  /// Assign an existing native socket to the socket.
  /*
   * This function opens the socket to hold an existing native socket.
   *
   * @param protocol An object specifying which protocol is to be used.
   *
   * @param native_socket A native socket.
   *
   * @throws boost::system::system_error Thrown on failure.
   */
  void assign(const protocol_type& protocol,
      const native_handle_type& native_socket)
  {
    boost::system::error_code ec;
    this->get_service().assign(this->get_implementation(),
        protocol, native_socket, ec);
    boost::asio::detail::throw_error(ec, "assign");
  }

  /// Assign an existing native socket to the socket.
  /*
   * This function opens the socket to hold an existing native socket.
   *
   * @param protocol An object specifying which protocol is to be used.
   *
   * @param native_socket A native socket.
   *
   * @param ec Set to indicate what error occurred, if any.
   */
  BOOST_ASIO_SYNC_OP_VOID assign(const protocol_type& protocol,
      const native_handle_type& native_socket, boost::system::error_code& ec)
  {
    this->get_service().assign(this->get_implementation(),
        protocol, native_socket, ec);
    BOOST_ASIO_SYNC_OP_VOID_RETURN(ec);
  }

  /// Determine whether the socket is open.
  bool is_open() const
  {
    return this->get_service().is_open(this->get_implementation());
  }

  /// Close the socket.
  /**
   * This function is used to close the socket. Any asynchronous send, receive
   * or connect operations will be cancelled immediately, and will complete
   * with the boost::asio::error::operation_aborted error.
   *
   * @throws boost::system::system_error Thrown on failure. Note that, even if
   * the function indicates an error, the underlying descriptor is closed.
   *
   * @note For portable behaviour with respect to graceful closure of a
   * connected socket, call shutdown() before closing the socket.
   */
  void close()
  {
    boost::system::error_code ec;
    this->get_service().close(this->get_implementation(), ec);
    boost::asio::detail::throw_error(ec, "close");
  }

  /// Close the socket.
  /**
   * This function is used to close the socket. Any asynchronous send, receive
   * or connect operations will be cancelled immediately, and will complete
   * with the boost::asio::error::operation_aborted error.
   *
   * @param ec Set to indicate what error occurred, if any. Note that, even if
   * the function indicates an error, the underlying descriptor is closed.
   *
   * @par Example
   * @code
   * boost::asio::ip::tcp::socket socket(io_context);
   * ...
   * boost::system::error_code ec;
   * socket.close(ec);
   * if (ec)
   * {
   *   // An error occurred.
   * }
   * @endcode
   *
   * @note For portable behaviour with respect to graceful closure of a
   * connected socket, call shutdown() before closing the socket.
   */
  BOOST_ASIO_SYNC_OP_VOID close(boost::system::error_code& ec)
  {
    this->get_service().close(this->get_implementation(), ec);
    BOOST_ASIO_SYNC_OP_VOID_RETURN(ec);
  }

  /// Release ownership of the underlying native socket.
  /**
   * This function causes all outstanding asynchronous connect, send and receive
   * operations to finish immediately, and the handlers for cancelled operations
   * will be passed the boost::asio::error::operation_aborted error. Ownership
   * of the native socket is then transferred to the caller.
   *
   * @throws boost::system::system_error Thrown on failure.
   *
   * @note This function is unsupported on Windows versions prior to Windows
   * 8.1, and will fail with boost::asio::error::operation_not_supported on
   * these platforms.
   */
#if defined(BOOST_ASIO_MSVC) && (BOOST_ASIO_MSVC >= 1400) \
  && (!defined(_WIN32_WINNT) || _WIN32_WINNT < 0x0603)
  __declspec(deprecated("This function always fails with "
        "operation_not_supported when used on Windows versions "
        "prior to Windows 8.1."))
#endif
  native_handle_type release()
  {
    boost::system::error_code ec;
    native_handle_type s = this->get_service().release(
        this->get_implementation(), ec);
    boost::asio::detail::throw_error(ec, "release");
    return s;
  }

  /// Release ownership of the underlying native socket.
  /**
   * This function causes all outstanding asynchronous connect, send and receive
   * operations to finish immediately, and the handlers for cancelled operations
   * will be passed the boost::asio::error::operation_aborted error. Ownership
   * of the native socket is then transferred to the caller.
   *
   * @param ec Set to indicate what error occurred, if any.
   *
   * @note This function is unsupported on Windows versions prior to Windows
   * 8.1, and will fail with boost::asio::error::operation_not_supported on
   * these platforms.
   */
#if defined(BOOST_ASIO_MSVC) && (BOOST_ASIO_MSVC >= 1400) \
  && (!defined(_WIN32_WINNT) || _WIN32_WINNT < 0x0603)
  __declspec(deprecated("This function always fails with "
        "operation_not_supported when used on Windows versions "
        "prior to Windows 8.1."))
#endif
  native_handle_type release(boost::system::error_code& ec)
  {
    return this->get_service().release(this->get_implementation(), ec);
  }

  /// Get the native socket representation.
  /**
   * This function may be used to obtain the underlying representation of the
   * socket. This is intended to allow access to native socket functionality
   * that is not otherwise provided.
   */
  native_handle_type native_handle()
  {
    return this->get_service().native_handle(this->get_implementation());
  }

  /// Cancel all asynchronous operations associated with the socket.
  /**
   * This function causes all outstanding asynchronous connect, send and receive
   * operations to finish immediately, and the handlers for cancelled operations
   * will be passed the boost::asio::error::operation_aborted error.
   *
   * @throws boost::system::system_error Thrown on failure.
   *
   * @note Calls to cancel() will always fail with
   * boost::asio::error::operation_not_supported when run on Windows XP, Windows
   * Server 2003, and earlier versions of Windows, unless
   * BOOST_ASIO_ENABLE_CANCELIO is defined. However, the CancelIo function has
   * two issues that should be considered before enabling its use:
   *
   * @li It will only cancel asynchronous operations that were initiated in the
   * current thread.
   *
   * @li It can appear to complete without error, but the request to cancel the
   * unfinished operations may be silently ignored by the operating system.
   * Whether it works or not seems to depend on the drivers that are installed.
   *
   * For portable cancellation, consider using one of the following
   * alternatives:
   *
   * @li Disable asio's I/O completion port backend by defining
   * BOOST_ASIO_DISABLE_IOCP.
   *
   * @li Use the close() function to simultaneously cancel the outstanding
   * operations and close the socket.
   *
   * When running on Windows Vista, Windows Server 2008, and later, the
   * CancelIoEx function is always used. This function does not have the
   * problems described above.
   */
#if defined(BOOST_ASIO_MSVC) && (BOOST_ASIO_MSVC >= 1400) \
  && (!defined(_WIN32_WINNT) || _WIN32_WINNT < 0x0600) \
  && !defined(BOOST_ASIO_ENABLE_CANCELIO)
  __declspec(deprecated("By default, this function always fails with "
        "operation_not_supported when used on Windows XP, Windows Server 2003, "
        "or earlier. Consult documentation for details."))
#endif
  void cancel()
  {
    boost::system::error_code ec;
    this->get_service().cancel(this->get_implementation(), ec);
    boost::asio::detail::throw_error(ec, "cancel");
  }

  /// Cancel all asynchronous operations associated with the socket.
  /**
   * This function causes all outstanding asynchronous connect, send and receive
   * operations to finish immediately, and the handlers for cancelled operations
   * will be passed the boost::asio::error::operation_aborted error.
   *
   * @param ec Set to indicate what error occurred, if any.
   *
   * @note Calls to cancel() will always fail with
   * boost::asio::error::operation_not_supported when run on Windows XP, Windows
   * Server 2003, and earlier versions of Windows, unless
   * BOOST_ASIO_ENABLE_CANCELIO is defined. However, the CancelIo function has
   * two issues that should be considered before enabling its use:
   *
   * @li It will only cancel asynchronous operations that were initiated in the
   * current thread.
   *
   * @li It can appear to complete without error, but the request to cancel the
   * unfinished operations may be silently ignored by the operating system.
   * Whether it works or not seems to depend on the drivers that are installed.
   *
   * For portable cancellation, consider using one of the following
   * alternatives:
   *
   * @li Disable asio's I/O completion port backend by defining
   * BOOST_ASIO_DISABLE_IOCP.
   *
   * @li Use the close() function to simultaneously cancel the outstanding
   * operations and close the socket.
   *
   * When running on Windows Vista, Windows Server 2008, and later, the
   * CancelIoEx function is always used. This function does not have the
   * problems described above.
   */
#if defined(BOOST_ASIO_MSVC) && (BOOST_ASIO_MSVC >= 1400) \
  && (!defined(_WIN32_WINNT) || _WIN32_WINNT < 0x0600) \
  && !defined(BOOST_ASIO_ENABLE_CANCELIO)
  __declspec(deprecated("By default, this function always fails with "
        "operation_not_supported when used on Windows XP, Windows Server 2003, "
        "or earlier. Consult documentation for details."))
#endif
  BOOST_ASIO_SYNC_OP_VOID cancel(boost::system::error_code& ec)
  {
    this->get_service().cancel(this->get_implementation(), ec);
    BOOST_ASIO_SYNC_OP_VOID_RETURN(ec);
  }

  /// Determine whether the socket is at the out-of-band data mark.
  /**
   * This function is used to check whether the socket input is currently
   * positioned at the out-of-band data mark.
   *
   * @return A bool indicating whether the socket is at the out-of-band data
   * mark.
   *
   * @throws boost::system::system_error Thrown on failure.
   */
  bool at_mark() const
  {
    boost::system::error_code ec;
    bool b = this->get_service().at_mark(this->get_implementation(), ec);
    boost::asio::detail::throw_error(ec, "at_mark");
    return b;
  }

  /// Determine whether the socket is at the out-of-band data mark.
  /**
   * This function is used to check whether the socket input is currently
   * positioned at the out-of-band data mark.
   *
   * @param ec Set to indicate what error occurred, if any.
   *
   * @return A bool indicating whether the socket is at the out-of-band data
   * mark.
   */
  bool at_mark(boost::system::error_code& ec) const
  {
    return this->get_service().at_mark(this->get_implementation(), ec);
  }

  /// Determine the number of bytes available for reading.
  /**
   * This function is used to determine the number of bytes that may be read
   * without blocking.
   *
   * @return The number of bytes that may be read without blocking, or 0 if an
   * error occurs.
   *
   * @throws boost::system::system_error Thrown on failure.
   */
  std::size_t available() const
  {
    boost::system::error_code ec;
    std::size_t s = this->get_service().available(
        this->get_implementation(), ec);
    boost::asio::detail::throw_error(ec, "available");
    return s;
  }

  /// Determine the number of bytes available for reading.
  /**
   * This function is used to determine the number of bytes that may be read
   * without blocking.
   *
   * @param ec Set to indicate what error occurred, if any.
   *
   * @return The number of bytes that may be read without blocking, or 0 if an
   * error occurs.
   */
  std::size_t available(boost::system::error_code& ec) const
  {
    return this->get_service().available(this->get_implementation(), ec);
  }

  /// Bind the socket to the given local endpoint.
  /**
   * This function binds the socket to the specified endpoint on the local
   * machine.
   *
   * @param endpoint An endpoint on the local machine to which the socket will
   * be bound.
   *
   * @throws boost::system::system_error Thrown on failure.
   *
   * @par Example
   * @code
   * boost::asio::ip::tcp::socket socket(io_context);
   * socket.open(boost::asio::ip::tcp::v4());
   * socket.bind(boost::asio::ip::tcp::endpoint(
   *       boost::asio::ip::tcp::v4(), 12345));
   * @endcode
   */
  void bind(const endpoint_type& endpoint)
  {
    boost::system::error_code ec;
    this->get_service().bind(this->get_implementation(), endpoint, ec);
    boost::asio::detail::throw_error(ec, "bind");
  }

  /// Bind the socket to the given local endpoint.
  /**
   * This function binds the socket to the specified endpoint on the local
   * machine.
   *
   * @param endpoint An endpoint on the local machine to which the socket will
   * be bound.
   *
   * @param ec Set to indicate what error occurred, if any.
   *
   * @par Example
   * @code
   * boost::asio::ip::tcp::socket socket(io_context);
   * socket.open(boost::asio::ip::tcp::v4());
   * boost::system::error_code ec;
   * socket.bind(boost::asio::ip::tcp::endpoint(
   *       boost::asio::ip::tcp::v4(), 12345), ec);
   * if (ec)
   * {
   *   // An error occurred.
   * }
   * @endcode
   */
  BOOST_ASIO_SYNC_OP_VOID bind(const endpoint_type& endpoint,
      boost::system::error_code& ec)
  {
    this->get_service().bind(this->get_implementation(), endpoint, ec);
    BOOST_ASIO_SYNC_OP_VOID_RETURN(ec);
  }

  /// Connect the socket to the specified endpoint.
  /**
   * This function is used to connect a socket to the specified remote endpoint.
   * The function call will block until the connection is successfully made or
   * an error occurs.
   *
   * The socket is automatically opened if it is not already open. If the
   * connect fails, and the socket was automatically opened, the socket is
   * not returned to the closed state.
   *
   * @param peer_endpoint The remote endpoint to which the socket will be
   * connected.
   *
   * @throws boost::system::system_error Thrown on failure.
   *
   * @par Example
   * @code
   * boost::asio::ip::tcp::socket socket(io_context);
   * boost::asio::ip::tcp::endpoint endpoint(
   *     boost::asio::ip::address::from_string("1.2.3.4"), 12345);
   * socket.connect(endpoint);
   * @endcode
   */
  void connect(const endpoint_type& peer_endpoint)
  {
    boost::system::error_code ec;
    if (!is_open())
    {
      this->get_service().open(this->get_implementation(),
          peer_endpoint.protocol(), ec);
      boost::asio::detail::throw_error(ec, "connect");
    }
    this->get_service().connect(this->get_implementation(), peer_endpoint, ec);
    boost::asio::detail::throw_error(ec, "connect");
  }

  /// Connect the socket to the specified endpoint.
  /**
   * This function is used to connect a socket to the specified remote endpoint.
   * The function call will block until the connection is successfully made or
   * an error occurs.
   *
   * The socket is automatically opened if it is not already open. If the
   * connect fails, and the socket was automatically opened, the socket is
   * not returned to the closed state.
   *
   * @param peer_endpoint The remote endpoint to which the socket will be
   * connected.
   *
   * @param ec Set to indicate what error occurred, if any.
   *
   * @par Example
   * @code
   * boost::asio::ip::tcp::socket socket(io_context);
   * boost::asio::ip::tcp::endpoint endpoint(
   *     boost::asio::ip::address::from_string("1.2.3.4"), 12345);
   * boost::system::error_code ec;
   * socket.connect(endpoint, ec);
   * if (ec)
   * {
   *   // An error occurred.
   * }
   * @endcode
   */
  BOOST_ASIO_SYNC_OP_VOID connect(const endpoint_type& peer_endpoint,
      boost::system::error_code& ec)
  {
    if (!is_open())
    {
      this->get_service().open(this->get_implementation(),
            peer_endpoint.protocol(), ec);
      if (ec)
      {
        BOOST_ASIO_SYNC_OP_VOID_RETURN(ec);
      }
    }

    this->get_service().connect(this->get_implementation(), peer_endpoint, ec);
    BOOST_ASIO_SYNC_OP_VOID_RETURN(ec);
  }

  /// Start an asynchronous connect.
  /**
   * This function is used to asynchronously connect a socket to the specified
   * remote endpoint. The function call always returns immediately.
   *
   * The socket is automatically opened if it is not already open. If the
   * connect fails, and the socket was automatically opened, the socket is
   * not returned to the closed state.
   *
   * @param peer_endpoint The remote endpoint to which the socket will be
   * connected. Copies will be made of the endpoint object as required.
   *
   * @param handler The handler to be called when the connection operation
   * completes. Copies will be made of the handler as required. The function
   * signature of the handler must be:
   * @code void handler(
   *   const boost::system::error_code& error // Result of operation
   * ); @endcode
   * Regardless of whether the asynchronous operation completes immediately or
   * not, the handler will not be invoked from within this function. Invocation
   * of the handler will be performed in a manner equivalent to using
   * boost::asio::io_context::post().
   *
   * @par Example
   * @code
   * void connect_handler(const boost::system::error_code& error)
   * {
   *   if (!error)
   *   {
   *     // Connect succeeded.
   *   }
   * }
   *
   * ...
   *
   * boost::asio::ip::tcp::socket socket(io_context);
   * boost::asio::ip::tcp::endpoint endpoint(
   *     boost::asio::ip::address::from_string("1.2.3.4"), 12345);
   * socket.async_connect(endpoint, connect_handler);
   * @endcode
   */
  template <typename ConnectHandler>
  BOOST_ASIO_INITFN_RESULT_TYPE(ConnectHandler,
      void (boost::system::error_code))
  async_connect(const endpoint_type& peer_endpoint,
      BOOST_ASIO_MOVE_ARG(ConnectHandler) handler)
  {
    // If you get an error on the following line it means that your handler does
    // not meet the documented type requirements for a ConnectHandler.
    BOOST_ASIO_CONNECT_HANDLER_CHECK(ConnectHandler, handler) type_check;

    if (!is_open())
    {
      boost::system::error_code ec;
      const protocol_type protocol = peer_endpoint.protocol();
      this->get_service().open(this->get_implementation(), protocol, ec);
      if (ec)
      {
        async_completion<ConnectHandler,
          void (boost::system::error_code)> init(handler);

        boost::asio::post(this->get_executor(),
            boost::asio::detail::bind_handler(
              BOOST_ASIO_MOVE_CAST(BOOST_ASIO_HANDLER_TYPE(
                ConnectHandler, void (boost::system::error_code)))(
                  init.completion_handler), ec));

        return init.result.get();
      }
    }

#if defined(BOOST_ASIO_ENABLE_OLD_SERVICES)
    return this->get_service().async_connect(this->get_implementation(),
        peer_endpoint, BOOST_ASIO_MOVE_CAST(ConnectHandler)(handler));
#else // defined(BOOST_ASIO_ENABLE_OLD_SERVICES)
    async_completion<ConnectHandler,
      void (boost::system::error_code)> init(handler);

    this->get_service().async_connect(
        this->get_implementation(), peer_endpoint, init.completion_handler);

    return init.result.get();
#endif // defined(BOOST_ASIO_ENABLE_OLD_SERVICES)
  }

  /// Set an option on the socket.
  /**
   * This function is used to set an option on the socket.
   *
   * @param option The new option value to be set on the socket.
   *
   * @throws boost::system::system_error Thrown on failure.
   *
   * @sa SettableSocketOption @n
   * boost::asio::socket_base::broadcast @n
   * boost::asio::socket_base::do_not_route @n
   * boost::asio::socket_base::keep_alive @n
   * boost::asio::socket_base::linger @n
   * boost::asio::socket_base::receive_buffer_size @n
   * boost::asio::socket_base::receive_low_watermark @n
   * boost::asio::socket_base::reuse_address @n
   * boost::asio::socket_base::send_buffer_size @n
   * boost::asio::socket_base::send_low_watermark @n
   * boost::asio::ip::multicast::join_group @n
   * boost::asio::ip::multicast::leave_group @n
   * boost::asio::ip::multicast::enable_loopback @n
   * boost::asio::ip::multicast::outbound_interface @n
   * boost::asio::ip::multicast::hops @n
   * boost::asio::ip::tcp::no_delay
   *
   * @par Example
   * Setting the IPPROTO_TCP/TCP_NODELAY option:
   * @code
   * boost::asio::ip::tcp::socket socket(io_context);
   * ...
   * boost::asio::ip::tcp::no_delay option(true);
   * socket.set_option(option);
   * @endcode
   */
  template <typename SettableSocketOption>
  void set_option(const SettableSocketOption& option)
  {
    boost::system::error_code ec;
    this->get_service().set_option(this->get_implementation(), option, ec);
    boost::asio::detail::throw_error(ec, "set_option");
  }

  /// Set an option on the socket.
  /**
   * This function is used to set an option on the socket.
   *
   * @param option The new option value to be set on the socket.
   *
   * @param ec Set to indicate what error occurred, if any.
   *
   * @sa SettableSocketOption @n
   * boost::asio::socket_base::broadcast @n
   * boost::asio::socket_base::do_not_route @n
   * boost::asio::socket_base::keep_alive @n
   * boost::asio::socket_base::linger @n
   * boost::asio::socket_base::receive_buffer_size @n
   * boost::asio::socket_base::receive_low_watermark @n
   * boost::asio::socket_base::reuse_address @n
   * boost::asio::socket_base::send_buffer_size @n
   * boost::asio::socket_base::send_low_watermark @n
   * boost::asio::ip::multicast::join_group @n
   * boost::asio::ip::multicast::leave_group @n
   * boost::asio::ip::multicast::enable_loopback @n
   * boost::asio::ip::multicast::outbound_interface @n
   * boost::asio::ip::multicast::hops @n
   * boost::asio::ip::tcp::no_delay
   *
   * @par Example
   * Setting the IPPROTO_TCP/TCP_NODELAY option:
   * @code
   * boost::asio::ip::tcp::socket socket(io_context);
   * ...
   * boost::asio::ip::tcp::no_delay option(true);
   * boost::system::error_code ec;
   * socket.set_option(option, ec);
   * if (ec)
   * {
   *   // An error occurred.
   * }
   * @endcode
   */
  template <typename SettableSocketOption>
  BOOST_ASIO_SYNC_OP_VOID set_option(const SettableSocketOption& option,
      boost::system::error_code& ec)
  {
    this->get_service().set_option(this->get_implementation(), option, ec);
    BOOST_ASIO_SYNC_OP_VOID_RETURN(ec);
  }

  /// Get an option from the socket.
  /**
   * This function is used to get the current value of an option on the socket.
   *
   * @param option The option value to be obtained from the socket.
   *
   * @throws boost::system::system_error Thrown on failure.
   *
   * @sa GettableSocketOption @n
   * boost::asio::socket_base::broadcast @n
   * boost::asio::socket_base::do_not_route @n
   * boost::asio::socket_base::keep_alive @n
   * boost::asio::socket_base::linger @n
   * boost::asio::socket_base::receive_buffer_size @n
   * boost::asio::socket_base::receive_low_watermark @n
   * boost::asio::socket_base::reuse_address @n
   * boost::asio::socket_base::send_buffer_size @n
   * boost::asio::socket_base::send_low_watermark @n
   * boost::asio::ip::multicast::join_group @n
   * boost::asio::ip::multicast::leave_group @n
   * boost::asio::ip::multicast::enable_loopback @n
   * boost::asio::ip::multicast::outbound_interface @n
   * boost::asio::ip::multicast::hops @n
   * boost::asio::ip::tcp::no_delay
   *
   * @par Example
   * Getting the value of the SOL_SOCKET/SO_KEEPALIVE option:
   * @code
   * boost::asio::ip::tcp::socket socket(io_context);
   * ...
   * boost::asio::ip::tcp::socket::keep_alive option;
   * socket.get_option(option);
   * bool is_set = option.value();
   * @endcode
   */
  template <typename GettableSocketOption>
  void get_option(GettableSocketOption& option) const
  {
    boost::system::error_code ec;
    this->get_service().get_option(this->get_implementation(), option, ec);
    boost::asio::detail::throw_error(ec, "get_option");
  }

  /// Get an option from the socket.
  /**
   * This function is used to get the current value of an option on the socket.
   *
   * @param option The option value to be obtained from the socket.
   *
   * @param ec Set to indicate what error occurred, if any.
   *
   * @sa GettableSocketOption @n
   * boost::asio::socket_base::broadcast @n
   * boost::asio::socket_base::do_not_route @n
   * boost::asio::socket_base::keep_alive @n
   * boost::asio::socket_base::linger @n
   * boost::asio::socket_base::receive_buffer_size @n
   * boost::asio::socket_base::receive_low_watermark @n
   * boost::asio::socket_base::reuse_address @n
   * boost::asio::socket_base::send_buffer_size @n
   * boost::asio::socket_base::send_low_watermark @n
   * boost::asio::ip::multicast::join_group @n
   * boost::asio::ip::multicast::leave_group @n
   * boost::asio::ip::multicast::enable_loopback @n
   * boost::asio::ip::multicast::outbound_interface @n
   * boost::asio::ip::multicast::hops @n
   * boost::asio::ip::tcp::no_delay
   *
   * @par Example
   * Getting the value of the SOL_SOCKET/SO_KEEPALIVE option:
   * @code
   * boost::asio::ip::tcp::socket socket(io_context);
   * ...
   * boost::asio::ip::tcp::socket::keep_alive option;
   * boost::system::error_code ec;
   * socket.get_option(option, ec);
   * if (ec)
   * {
   *   // An error occurred.
   * }
   * bool is_set = option.value();
   * @endcode
   */
  template <typename GettableSocketOption>
  BOOST_ASIO_SYNC_OP_VOID get_option(GettableSocketOption& option,
      boost::system::error_code& ec) const
  {
    this->get_service().get_option(this->get_implementation(), option, ec);
    BOOST_ASIO_SYNC_OP_VOID_RETURN(ec);
  }

  /// Perform an IO control command on the socket.
  /**
   * This function is used to execute an IO control command on the socket.
   *
   * @param command The IO control command to be performed on the socket.
   *
   * @throws boost::system::system_error Thrown on failure.
   *
   * @sa IoControlCommand @n
   * boost::asio::socket_base::bytes_readable @n
   * boost::asio::socket_base::non_blocking_io
   *
   * @par Example
   * Getting the number of bytes ready to read:
   * @code
   * boost::asio::ip::tcp::socket socket(io_context);
   * ...
   * boost::asio::ip::tcp::socket::bytes_readable command;
   * socket.io_control(command);
   * std::size_t bytes_readable = command.get();
   * @endcode
   */
  template <typename IoControlCommand>
  void io_control(IoControlCommand& command)
  {
    boost::system::error_code ec;
    this->get_service().io_control(this->get_implementation(), command, ec);
    boost::asio::detail::throw_error(ec, "io_control");
  }

  /// Perform an IO control command on the socket.
  /**
   * This function is used to execute an IO control command on the socket.
   *
   * @param command The IO control command to be performed on the socket.
   *
   * @param ec Set to indicate what error occurred, if any.
   *
   * @sa IoControlCommand @n
   * boost::asio::socket_base::bytes_readable @n
   * boost::asio::socket_base::non_blocking_io
   *
   * @par Example
   * Getting the number of bytes ready to read:
   * @code
   * boost::asio::ip::tcp::socket socket(io_context);
   * ...
   * boost::asio::ip::tcp::socket::bytes_readable command;
   * boost::system::error_code ec;
   * socket.io_control(command, ec);
   * if (ec)
   * {
   *   // An error occurred.
   * }
   * std::size_t bytes_readable = command.get();
   * @endcode
   */
  template <typename IoControlCommand>
  BOOST_ASIO_SYNC_OP_VOID io_control(IoControlCommand& command,
      boost::system::error_code& ec)
  {
    this->get_service().io_control(this->get_implementation(), command, ec);
    BOOST_ASIO_SYNC_OP_VOID_RETURN(ec);
  }

  /// Gets the non-blocking mode of the socket.
  /**
   * @returns @c true if the socket's synchronous operations will fail with
   * boost::asio::error::would_block if they are unable to perform the requested
   * operation immediately. If @c false, synchronous operations will block
   * until complete.
   *
   * @note The non-blocking mode has no effect on the behaviour of asynchronous
   * operations. Asynchronous operations will never fail with the error
   * boost::asio::error::would_block.
   */
  bool non_blocking() const
  {
    return this->get_service().non_blocking(this->get_implementation());
  }

  /// Sets the non-blocking mode of the socket.
  /**
   * @param mode If @c true, the socket's synchronous operations will fail with
   * boost::asio::error::would_block if they are unable to perform the requested
   * operation immediately. If @c false, synchronous operations will block
   * until complete.
   *
   * @throws boost::system::system_error Thrown on failure.
   *
   * @note The non-blocking mode has no effect on the behaviour of asynchronous
   * operations. Asynchronous operations will never fail with the error
   * boost::asio::error::would_block.
   */
  void non_blocking(bool mode)
  {
    boost::system::error_code ec;
    this->get_service().non_blocking(this->get_implementation(), mode, ec);
    boost::asio::detail::throw_error(ec, "non_blocking");
  }

  /// Sets the non-blocking mode of the socket.
  /**
   * @param mode If @c true, the socket's synchronous operations will fail with
   * boost::asio::error::would_block if they are unable to perform the requested
   * operation immediately. If @c false, synchronous operations will block
   * until complete.
   *
   * @param ec Set to indicate what error occurred, if any.
   *
   * @note The non-blocking mode has no effect on the behaviour of asynchronous
   * operations. Asynchronous operations will never fail with the error
   * boost::asio::error::would_block.
   */
  BOOST_ASIO_SYNC_OP_VOID non_blocking(
      bool mode, boost::system::error_code& ec)
  {
    this->get_service().non_blocking(this->get_implementation(), mode, ec);
    BOOST_ASIO_SYNC_OP_VOID_RETURN(ec);
  }

  /// Gets the non-blocking mode of the native socket implementation.
  /**
   * This function is used to retrieve the non-blocking mode of the underlying
   * native socket. This mode has no effect on the behaviour of the socket
   * object's synchronous operations.
   *
   * @returns @c true if the underlying socket is in non-blocking mode and
   * direct system calls may fail with boost::asio::error::would_block (or the
   * equivalent system error).
   *
   * @note The current non-blocking mode is cached by the socket object.
   * Consequently, the return value may be incorrect if the non-blocking mode
   * was set directly on the native socket.
   *
   * @par Example
   * This function is intended to allow the encapsulation of arbitrary
   * non-blocking system calls as asynchronous operations, in a way that is
   * transparent to the user of the socket object. The following example
   * illustrates how Linux's @c sendfile system call might be encapsulated:
   * @code template <typename Handler>
   * struct sendfile_op
   * {
   *   tcp::socket& sock_;
   *   int fd_;
   *   Handler handler_;
   *   off_t offset_;
   *   std::size_t total_bytes_transferred_;
   *
   *   // Function call operator meeting WriteHandler requirements.
   *   // Used as the handler for the async_write_some operation.
   *   void operator()(boost::system::error_code ec, std::size_t)
   *   {
   *     // Put the underlying socket into non-blocking mode.
   *     if (!ec)
   *       if (!sock_.native_non_blocking())
   *         sock_.native_non_blocking(true, ec);
   *
   *     if (!ec)
   *     {
   *       for (;;)
   *       {
   *         // Try the system call.
   *         errno = 0;
   *         int n = ::sendfile(sock_.native_handle(), fd_, &offset_, 65536);
   *         ec = boost::system::error_code(n < 0 ? errno : 0,
   *             boost::asio::error::get_system_category());
   *         total_bytes_transferred_ += ec ? 0 : n;
   *
   *         // Retry operation immediately if interrupted by signal.
   *         if (ec == boost::asio::error::interrupted)
   *           continue;
   *
   *         // Check if we need to run the operation again.
   *         if (ec == boost::asio::error::would_block
   *             || ec == boost::asio::error::try_again)
   *         {
   *           // We have to wait for the socket to become ready again.
   *           sock_.async_wait(tcp::socket::wait_write, *this);
   *           return;
   *         }
   *
   *         if (ec || n == 0)
   *         {
   *           // An error occurred, or we have reached the end of the file.
   *           // Either way we must exit the loop so we can call the handler.
   *           break;
   *         }
   *
   *         // Loop around to try calling sendfile again.
   *       }
   *     }
   *
   *     // Pass result back to user's handler.
   *     handler_(ec, total_bytes_transferred_);
   *   }
   * };
   *
   * template <typename Handler>
   * void async_sendfile(tcp::socket& sock, int fd, Handler h)
   * {
   *   sendfile_op<Handler> op = { sock, fd, h, 0, 0 };
   *   sock.async_wait(tcp::socket::wait_write, op);
   * } @endcode
   */
  bool native_non_blocking() const
  {
    return this->get_service().native_non_blocking(this->get_implementation());
  }

  /// Sets the non-blocking mode of the native socket implementation.
  /**
   * This function is used to modify the non-blocking mode of the underlying
   * native socket. It has no effect on the behaviour of the socket object's
   * synchronous operations.
   *
   * @param mode If @c true, the underlying socket is put into non-blocking
   * mode and direct system calls may fail with boost::asio::error::would_block
   * (or the equivalent system error).
   *
   * @throws boost::system::system_error Thrown on failure. If the @c mode is
   * @c false, but the current value of @c non_blocking() is @c true, this
   * function fails with boost::asio::error::invalid_argument, as the
   * combination does not make sense.
   *
   * @par Example
   * This function is intended to allow the encapsulation of arbitrary
   * non-blocking system calls as asynchronous operations, in a way that is
   * transparent to the user of the socket object. The following example
   * illustrates how Linux's @c sendfile system call might be encapsulated:
   * @code template <typename Handler>
   * struct sendfile_op
   * {
   *   tcp::socket& sock_;
   *   int fd_;
   *   Handler handler_;
   *   off_t offset_;
   *   std::size_t total_bytes_transferred_;
   *
   *   // Function call operator meeting WriteHandler requirements.
   *   // Used as the handler for the async_write_some operation.
   *   void operator()(boost::system::error_code ec, std::size_t)
   *   {
   *     // Put the underlying socket into non-blocking mode.
   *     if (!ec)
   *       if (!sock_.native_non_blocking())
   *         sock_.native_non_blocking(true, ec);
   *
   *     if (!ec)
   *     {
   *       for (;;)
   *       {
   *         // Try the system call.
   *         errno = 0;
   *         int n = ::sendfile(sock_.native_handle(), fd_, &offset_, 65536);
   *         ec = boost::system::error_code(n < 0 ? errno : 0,
   *             boost::asio::error::get_system_category());
   *         total_bytes_transferred_ += ec ? 0 : n;
   *
   *         // Retry operation immediately if interrupted by signal.
   *         if (ec == boost::asio::error::interrupted)
   *           continue;
   *
   *         // Check if we need to run the operation again.
   *         if (ec == boost::asio::error::would_block
   *             || ec == boost::asio::error::try_again)
   *         {
   *           // We have to wait for the socket to become ready again.
   *           sock_.async_wait(tcp::socket::wait_write, *this);
   *           return;
   *         }
   *
   *         if (ec || n == 0)
   *         {
   *           // An error occurred, or we have reached the end of the file.
   *           // Either way we must exit the loop so we can call the handler.
   *           break;
   *         }
   *
   *         // Loop around to try calling sendfile again.
   *       }
   *     }
   *
   *     // Pass result back to user's handler.
   *     handler_(ec, total_bytes_transferred_);
   *   }
   * };
   *
   * template <typename Handler>
   * void async_sendfile(tcp::socket& sock, int fd, Handler h)
   * {
   *   sendfile_op<Handler> op = { sock, fd, h, 0, 0 };
   *   sock.async_wait(tcp::socket::wait_write, op);
   * } @endcode
   */
  void native_non_blocking(bool mode)
  {
    boost::system::error_code ec;
    this->get_service().native_non_blocking(
        this->get_implementation(), mode, ec);
    boost::asio::detail::throw_error(ec, "native_non_blocking");
  }

  /// Sets the non-blocking mode of the native socket implementation.
  /**
   * This function is used to modify the non-blocking mode of the underlying
   * native socket. It has no effect on the behaviour of the socket object's
   * synchronous operations.
   *
   * @param mode If @c true, the underlying socket is put into non-blocking
   * mode and direct system calls may fail with boost::asio::error::would_block
   * (or the equivalent system error).
   *
   * @param ec Set to indicate what error occurred, if any. If the @c mode is
   * @c false, but the current value of @c non_blocking() is @c true, this
   * function fails with boost::asio::error::invalid_argument, as the
   * combination does not make sense.
   *
   * @par Example
   * This function is intended to allow the encapsulation of arbitrary
   * non-blocking system calls as asynchronous operations, in a way that is
   * transparent to the user of the socket object. The following example
   * illustrates how Linux's @c sendfile system call might be encapsulated:
   * @code template <typename Handler>
   * struct sendfile_op
   * {
   *   tcp::socket& sock_;
   *   int fd_;
   *   Handler handler_;
   *   off_t offset_;
   *   std::size_t total_bytes_transferred_;
   *
   *   // Function call operator meeting WriteHandler requirements.
   *   // Used as the handler for the async_write_some operation.
   *   void operator()(boost::system::error_code ec, std::size_t)
   *   {
   *     // Put the underlying socket into non-blocking mode.
   *     if (!ec)
   *       if (!sock_.native_non_blocking())
   *         sock_.native_non_blocking(true, ec);
   *
   *     if (!ec)
   *     {
   *       for (;;)
   *       {
   *         // Try the system call.
   *         errno = 0;
   *         int n = ::sendfile(sock_.native_handle(), fd_, &offset_, 65536);
   *         ec = boost::system::error_code(n < 0 ? errno : 0,
   *             boost::asio::error::get_system_category());
   *         total_bytes_transferred_ += ec ? 0 : n;
   *
   *         // Retry operation immediately if interrupted by signal.
   *         if (ec == boost::asio::error::interrupted)
   *           continue;
   *
   *         // Check if we need to run the operation again.
   *         if (ec == boost::asio::error::would_block
   *             || ec == boost::asio::error::try_again)
   *         {
   *           // We have to wait for the socket to become ready again.
   *           sock_.async_wait(tcp::socket::wait_write, *this);
   *           return;
   *         }
   *
   *         if (ec || n == 0)
   *         {
   *           // An error occurred, or we have reached the end of the file.
   *           // Either way we must exit the loop so we can call the handler.
   *           break;
   *         }
   *
   *         // Loop around to try calling sendfile again.
   *       }
   *     }
   *
   *     // Pass result back to user's handler.
   *     handler_(ec, total_bytes_transferred_);
   *   }
   * };
   *
   * template <typename Handler>
   * void async_sendfile(tcp::socket& sock, int fd, Handler h)
   * {
   *   sendfile_op<Handler> op = { sock, fd, h, 0, 0 };
   *   sock.async_wait(tcp::socket::wait_write, op);
   * } @endcode
   */
  BOOST_ASIO_SYNC_OP_VOID native_non_blocking(
      bool mode, boost::system::error_code& ec)
  {
    this->get_service().native_non_blocking(
        this->get_implementation(), mode, ec);
    BOOST_ASIO_SYNC_OP_VOID_RETURN(ec);
  }

  /// Get the local endpoint of the socket.
  /**
   * This function is used to obtain the locally bound endpoint of the socket.
   *
   * @returns An object that represents the local endpoint of the socket.
   *
   * @throws boost::system::system_error Thrown on failure.
   *
   * @par Example
   * @code
   * boost::asio::ip::tcp::socket socket(io_context);
   * ...
   * boost::asio::ip::tcp::endpoint endpoint = socket.local_endpoint();
   * @endcode
   */
  endpoint_type local_endpoint() const
  {
    boost::system::error_code ec;
    endpoint_type ep = this->get_service().local_endpoint(
        this->get_implementation(), ec);
    boost::asio::detail::throw_error(ec, "local_endpoint");
    return ep;
  }

  /// Get the local endpoint of the socket.
  /**
   * This function is used to obtain the locally bound endpoint of the socket.
   *
   * @param ec Set to indicate what error occurred, if any.
   *
   * @returns An object that represents the local endpoint of the socket.
   * Returns a default-constructed endpoint object if an error occurred.
   *
   * @par Example
   * @code
   * boost::asio::ip::tcp::socket socket(io_context);
   * ...
   * boost::system::error_code ec;
   * boost::asio::ip::tcp::endpoint endpoint = socket.local_endpoint(ec);
   * if (ec)
   * {
   *   // An error occurred.
   * }
   * @endcode
   */
  endpoint_type local_endpoint(boost::system::error_code& ec) const
  {
    return this->get_service().local_endpoint(this->get_implementation(), ec);
  }

  /// Get the remote endpoint of the socket.
  /**
   * This function is used to obtain the remote endpoint of the socket.
   *
   * @returns An object that represents the remote endpoint of the socket.
   *
   * @throws boost::system::system_error Thrown on failure.
   *
   * @par Example
   * @code
   * boost::asio::ip::tcp::socket socket(io_context);
   * ...
   * boost::asio::ip::tcp::endpoint endpoint = socket.remote_endpoint();
   * @endcode
   */
  endpoint_type remote_endpoint() const
  {
    boost::system::error_code ec;
    endpoint_type ep = this->get_service().remote_endpoint(
        this->get_implementation(), ec);
    boost::asio::detail::throw_error(ec, "remote_endpoint");
    return ep;
  }

  /// Get the remote endpoint of the socket.
  /**
   * This function is used to obtain the remote endpoint of the socket.
   *
   * @param ec Set to indicate what error occurred, if any.
   *
   * @returns An object that represents the remote endpoint of the socket.
   * Returns a default-constructed endpoint object if an error occurred.
   *
   * @par Example
   * @code
   * boost::asio::ip::tcp::socket socket(io_context);
   * ...
   * boost::system::error_code ec;
   * boost::asio::ip::tcp::endpoint endpoint = socket.remote_endpoint(ec);
   * if (ec)
   * {
   *   // An error occurred.
   * }
   * @endcode
   */
  endpoint_type remote_endpoint(boost::system::error_code& ec) const
  {
    return this->get_service().remote_endpoint(this->get_implementation(), ec);
  }

  /// Disable sends or receives on the socket.
  /**
   * This function is used to disable send operations, receive operations, or
   * both.
   *
   * @param what Determines what types of operation will no longer be allowed.
   *
   * @throws boost::system::system_error Thrown on failure.
   *
   * @par Example
   * Shutting down the send side of the socket:
   * @code
   * boost::asio::ip::tcp::socket socket(io_context);
   * ...
   * socket.shutdown(boost::asio::ip::tcp::socket::shutdown_send);
   * @endcode
   */
  void shutdown(shutdown_type what)
  {
    boost::system::error_code ec;
    this->get_service().shutdown(this->get_implementation(), what, ec);
    boost::asio::detail::throw_error(ec, "shutdown");
  }

  /// Disable sends or receives on the socket.
  /**
   * This function is used to disable send operations, receive operations, or
   * both.
   *
   * @param what Determines what types of operation will no longer be allowed.
   *
   * @param ec Set to indicate what error occurred, if any.
   *
   * @par Example
   * Shutting down the send side of the socket:
   * @code
   * boost::asio::ip::tcp::socket socket(io_context);
   * ...
   * boost::system::error_code ec;
   * socket.shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
   * if (ec)
   * {
   *   // An error occurred.
   * }
   * @endcode
   */
  BOOST_ASIO_SYNC_OP_VOID shutdown(shutdown_type what,
      boost::system::error_code& ec)
  {
    this->get_service().shutdown(this->get_implementation(), what, ec);
    BOOST_ASIO_SYNC_OP_VOID_RETURN(ec);
  }

  /// Wait for the socket to become ready to read, ready to write, or to have
  /// pending error conditions.
  /**
   * This function is used to perform a blocking wait for a socket to enter
   * a ready to read, write or error condition state.
   *
   * @param w Specifies the desired socket state.
   *
   * @par Example
   * Waiting for a socket to become readable.
   * @code
   * boost::asio::ip::tcp::socket socket(io_context);
   * ...
   * socket.wait(boost::asio::ip::tcp::socket::wait_read);
   * @endcode
   */
  void wait(wait_type w)
  {
    boost::system::error_code ec;
    this->get_service().wait(this->get_implementation(), w, ec);
    boost::asio::detail::throw_error(ec, "wait");
  }

  /// Wait for the socket to become ready to read, ready to write, or to have
  /// pending error conditions.
  /**
   * This function is used to perform a blocking wait for a socket to enter
   * a ready to read, write or error condition state.
   *
   * @param w Specifies the desired socket state.
   *
   * @param ec Set to indicate what error occurred, if any.
   *
   * @par Example
   * Waiting for a socket to become readable.
   * @code
   * boost::asio::ip::tcp::socket socket(io_context);
   * ...
   * boost::system::error_code ec;
   * socket.wait(boost::asio::ip::tcp::socket::wait_read, ec);
   * @endcode
   */
  BOOST_ASIO_SYNC_OP_VOID wait(wait_type w, boost::system::error_code& ec)
  {
    this->get_service().wait(this->get_implementation(), w, ec);
    BOOST_ASIO_SYNC_OP_VOID_RETURN(ec);
  }

  /// Asynchronously wait for the socket to become ready to read, ready to
  /// write, or to have pending error conditions.
  /**
   * This function is used to perform an asynchronous wait for a socket to enter
   * a ready to read, write or error condition state.
   *
   * @param w Specifies the desired socket state.
   *
   * @param handler The handler to be called when the wait operation completes.
   * Copies will be made of the handler as required. The function signature of
   * the handler must be:
   * @code void handler(
   *   const boost::system::error_code& error // Result of operation
   * ); @endcode
   * Regardless of whether the asynchronous operation completes immediately or
   * not, the handler will not be invoked from within this function. Invocation
   * of the handler will be performed in a manner equivalent to using
   * boost::asio::io_context::post().
   *
   * @par Example
   * @code
   * void wait_handler(const boost::system::error_code& error)
   * {
   *   if (!error)
   *   {
   *     // Wait succeeded.
   *   }
   * }
   *
   * ...
   *
   * boost::asio::ip::tcp::socket socket(io_context);
   * ...
   * socket.async_wait(boost::asio::ip::tcp::socket::wait_read, wait_handler);
   * @endcode
   */
  template <typename WaitHandler>
  BOOST_ASIO_INITFN_RESULT_TYPE(WaitHandler,
      void (boost::system::error_code))
  async_wait(wait_type w, BOOST_ASIO_MOVE_ARG(WaitHandler) handler)
  {
    // If you get an error on the following line it means that your handler does
    // not meet the documented type requirements for a WaitHandler.
    BOOST_ASIO_WAIT_HANDLER_CHECK(WaitHandler, handler) type_check;

#if defined(BOOST_ASIO_ENABLE_OLD_SERVICES)
    return this->get_service().async_wait(this->get_implementation(),
        w, BOOST_ASIO_MOVE_CAST(WaitHandler)(handler));
#else // defined(BOOST_ASIO_ENABLE_OLD_SERVICES)
    async_completion<WaitHandler,
      void (boost::system::error_code)> init(handler);

    this->get_service().async_wait(this->get_implementation(),
        w, init.completion_handler);

    return init.result.get();
#endif // defined(BOOST_ASIO_ENABLE_OLD_SERVICES)
  }

protected:
  /// Protected destructor to prevent deletion through this type.
  /**
   * This function destroys the socket, cancelling any outstanding asynchronous
   * operations associated with the socket as if by calling @c cancel.
   */
  ~basic_socket_ext()
  {
  }

private:
  // Disallow copying and assignment.
  basic_socket_ext(const basic_socket_ext&) BOOST_ASIO_DELETED;
  basic_socket_ext& operator=(const basic_socket_ext&) BOOST_ASIO_DELETED;
};

} // namespace asio
} // namespace boost

#include <boost/asio/detail/pop_options.hpp>

#if !defined(BOOST_ASIO_ENABLE_OLD_SERVICES)
# undef BOOST_ASIO_SVC_T
#endif // !defined(BOOST_ASIO_ENABLE_OLD_SERVICES)

#endif // BOOST_ASIO_BASIC_SOCKET_EXT_HPP
