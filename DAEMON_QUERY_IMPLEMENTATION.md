# Daemon Query Implementation for Dynamic Mixin

## Overview

This implementation adds daemon query functionality for dynamic mixin calculation in the Fuego blockchain. The system allows wallets to query the daemon for available outputs and receive recommendations for optimal ring sizes based on actual blockchain data.

## Components Implemented

### 1. RPC Command Definition
- **File**: `src/Rpc/CoreRpcServerCommandsDefinitions.h`
- **Command**: `COMMAND_RPC_GET_OUTPUT_AVAILABILITY`
- **Purpose**: Defines the request/response structure for output availability queries
- **Features**:
  - Query multiple amounts at once
  - Specify minimum ring size requirement
  - Returns available output counts per amount
  - Provides recommended ring size and privacy level

### 2. Daemon RPC Handler
- **File**: `src/Rpc/RpcServer.cpp`
- **Handler**: `on_get_output_availability`
- **Endpoint**: `/get_output_availability.bin`
- **Features**:
  - Queries actual output availability from blockchain
  - Uses existing `get_random_outs_for_amounts` for data
  - Integrates with `DynamicRingSizeCalculator`
  - Provides detailed privacy level descriptions
  - Handles block version compatibility

### 3. INode Interface Extension
- **File**: `include/INode.h`
- **Method**: `getOutputAvailability`
- **Purpose**: Defines the interface for output availability queries
- **Features**:
  - Async callback-based interface
  - Returns output availability data
  - Provides recommended ring size and privacy level

### 4. NodeRpcProxy Implementation
- **File**: `src/NodeRpcProxy/NodeRpcProxy.cpp`
- **Methods**: `getOutputAvailability`, `doGetOutputAvailability`
- **Purpose**: Implements the INode interface for RPC communication
- **Features**:
  - Binary RPC communication with daemon
  - Error handling and status reporting
  - Async callback support

### 5. Wallet Integration
- **File**: `src/WalletLegacy/WalletTransactionSender.cpp`
- **Method**: `calculateDynamicRingSize`
- **Purpose**: Uses daemon query for dynamic ring size calculation
- **Features**:
  - Queries daemon for actual output availability
  - Falls back to heuristic calculation if daemon query fails
  - Maintains backward compatibility
  - Integrates with existing dynamic ring size logic

### 6. Daemon Console Command
- **File**: `src/Daemon/DaemonCommandsHandler.cpp`
- **Command**: `query_outputs`
- **Purpose**: Allows manual testing of output availability
- **Usage**: `query_outputs <amount1> [amount2] [amount3] ...`
- **Features**:
  - Interactive output availability analysis
  - Privacy level recommendations
  - Detailed reporting per amount
  - Warning for insufficient outputs

## API Usage

### RPC Request
```json
{
  "amounts": [1000000000, 5000000000],
  "min_ring_size": 8
}
```

### RPC Response
```json
{
  "outputs": [
    {
      "amount": 1000000000,
      "available_count": 20,
      "description": "Maximum Privacy Available"
    },
    {
      "amount": 5000000000,
      "available_count": 15,
      "description": "Strong Privacy Available"
    }
  ],
  "recommended_ring_size": 18,
  "privacy_level": "Maximum Privacy (Ring Size 18)",
  "status": "OK"
}
```

### Console Command
```bash
query_outputs 1000000000 5000000000
```

## Privacy Levels

The system provides the following privacy levels based on available outputs:

- **Maximum Privacy** (18+ outputs): Ring Size 18
- **Strong Privacy** (15+ outputs): Ring Size 15
- **Better Privacy** (12+ outputs): Ring Size 12
- **Good Privacy** (10+ outputs): Ring Size 10
- **Enhanced Privacy** (8+ outputs): Ring Size 8
- **Limited Privacy** (1-7 outputs): Ring Size 1-7
- **No Privacy** (0 outputs): Error condition

## Block Version Compatibility

- **BlockMajorVersion 10+**: Full dynamic mixin support
- **Older versions**: Falls back to static ring size (2)

## Error Handling

- **Insufficient outputs**: Returns error with recommendation to run wallet optimizer
- **Daemon query failure**: Falls back to heuristic calculation
- **Invalid amounts**: Returns appropriate error messages
- **Network issues**: Handles gracefully with fallback mechanisms

## Security Considerations

- **Minimum ring size enforcement**: Never goes below ring size 8 for BlockMajorVersion 10+
- **Privacy preservation**: Always aims for maximum achievable privacy
- **Fallback mechanisms**: Ensures transactions can still be created even if optimal ring size unavailable
- **Error transparency**: Clear error messages help users understand issues

## Performance Impact

- **Minimal overhead**: Uses existing `get_random_outs_for_amounts` infrastructure
- **Efficient queries**: Batches multiple amounts in single request
- **Caching potential**: Can be extended with output availability caching
- **Async support**: Non-blocking implementation for wallet operations

## Future Enhancements

1. **Output availability caching**: Cache results to reduce daemon queries
2. **Real-time updates**: Subscribe to blockchain changes for live updates
3. **Advanced analytics**: Network-wide privacy analysis
4. **User preferences**: Allow users to set privacy vs. speed preferences
5. **Batch optimization**: Optimize multiple transactions together

## Testing

The implementation includes:
- Unit tests for `DynamicRingSizeCalculator`
- Integration tests for RPC endpoints
- Console command for manual testing
- Error condition testing
- Performance benchmarking

## Conclusion

This implementation provides a robust foundation for dynamic mixin calculation with real-time daemon queries. It maintains backward compatibility while significantly improving privacy through intelligent ring size selection based on actual blockchain data.