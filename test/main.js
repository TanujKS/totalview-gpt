#!/usr/bin/env node

const readline = require('readline');
const http = require('http');

// Configuration
const SERVER_HOST = process.env.SERVER_HOST || 'localhost';
const SERVER_PORT = process.env.SERVER_PORT || 8080;
const SERVER_URL = `http://${SERVER_HOST}:${SERVER_PORT}`;

// Parse command line arguments
const args = process.argv.slice(2);
let systemMessage = '';
let initialUserMessage = '';
let conversationId = '';

for (let i = 0; i < args.length; i++) {
  if (args[i] === '--system' || args[i] === '-s') {
    systemMessage = args[++i] || '';
  } else if (args[i] === '--message' || args[i] === '-m') {
    initialUserMessage = args[++i] || '';
  } else if (args[i] === '--conversation' || args[i] === '-c') {
    conversationId = args[++i] || '';
  } else if (args[i] === '--help' || args[i] === '-h') {
    console.log(`
Usage: node main.js [options]

Options:
  -s, --system <message>      Set system message (e.g., "You are a network administrator")
  -m, --message <message>      Send initial message and exit
  -c, --conversation <id>      Conversation ID for maintaining context
  -h, --help                   Show this help message

Environment variables:
  SERVER_HOST                  Backend server host (default: localhost)
  SERVER_PORT                  Backend server port (default: 8080)

Examples:
  node main.js -s "You are a network administrator"
  node main.js -s "You are a network administrator" -m "What is a router?"
  node main.js -c "conv-123" -s "You are a network administrator"
`);
    process.exit(0);
  }
}

// Conversation history
const conversationHistory = [];

// Create readline interface
const rl = readline.createInterface({
  input: process.stdin,
  output: process.stdout,
  prompt: '> '
});

// Function to send request to backend
function sendChatRequest(systemMsg, userMsg, context) {
  return new Promise((resolve, reject) => {
    const postData = JSON.stringify({
      system_message: systemMsg,
      user_message: userMsg,
      conversation_id: conversationId || undefined,
      context: context || {}
    });

    const options = {
      hostname: SERVER_HOST,
      port: SERVER_PORT,
      path: '/chat',
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
        'Content-Length': Buffer.byteLength(postData)
      }
    };

    const req = http.request(options, (res) => {
      let data = '';

      res.on('data', (chunk) => {
        data += chunk;
      });

      res.on('end', () => {
        if (res.statusCode !== 200) {
          try {
            const error = JSON.parse(data);
            reject(new Error(`Server error (${res.statusCode}): ${error.error || data}`));
          } catch {
            reject(new Error(`Server error (${res.statusCode}): ${data}`));
          }
          return;
        }

        try {
          const response = JSON.parse(data);
          resolve(response);
        } catch (e) {
          reject(new Error(`Failed to parse response: ${e.message}`));
        }
      });
    });

    req.on('error', (e) => {
      reject(new Error(`Request failed: ${e.message}`));
    });

    req.write(postData);
    req.end();
  });
}

// Function to display assistant response
function displayResponse(response) {
  console.log('\n' + '='.repeat(60));
  console.log('Assistant:');
  console.log('='.repeat(60));
  console.log(response.assistant_message);
  console.log('='.repeat(60) + '\n');
  
  // Update conversation ID if provided
  if (response.conversation_id) {
    conversationId = response.conversation_id;
  }
  
  // Add to conversation history
  conversationHistory.push({
    role: 'user',
    content: '' // Will be set by caller
  });
  conversationHistory.push({
    role: 'assistant',
    content: response.assistant_message
  });
}

// Function to handle a chat interaction
async function handleChat(userMessage) {
  if (!userMessage.trim()) {
    return;
  }

  // Build context from conversation history
  const context = {
    messages: conversationHistory.slice(0, -1) // Exclude the last entry (will be current user message)
  };

  // Add current user message to history temporarily
  const currentUserMsg = { role: 'user', content: userMessage };
  
  try {
    process.stdout.write('\nThinking... ');
    const response = await sendChatRequest(systemMessage, userMessage, context);
    process.stdout.write('\r' + ' '.repeat(20) + '\r'); // Clear "Thinking..."
    
    displayResponse(response);
    
    // Update the last entry in history with actual user message
    if (conversationHistory.length > 0 && conversationHistory[conversationHistory.length - 1].role === 'user') {
      conversationHistory[conversationHistory.length - 1] = currentUserMsg;
    } else {
      conversationHistory.push(currentUserMsg);
    }
    
    rl.prompt();
  } catch (error) {
    console.error('\nError:', error.message);
    rl.prompt();
  }
}

// Main function
async function main() {
  console.log('Chat CLI - Connecting to', SERVER_URL);
  console.log('Type your messages (or "exit" to quit, "clear" to clear history)\n');

  // Test connection with health check
  try {
    const healthCheck = await new Promise((resolve, reject) => {
      const req = http.get(`${SERVER_URL}/health`, (res) => {
        let data = '';
        res.on('data', (chunk) => { data += chunk; });
        res.on('end', () => {
          if (res.statusCode === 200) {
            resolve(true);
          } else {
            reject(new Error(`Health check failed: ${res.statusCode}`));
          }
        });
      });
      req.on('error', reject);
      req.setTimeout(3000, () => {
        req.destroy();
        reject(new Error('Health check timeout'));
      });
    });
    console.log('✓ Connected to server\n');
  } catch (error) {
    console.error('✗ Failed to connect to server:', error.message);
    console.error(`Make sure the server is running on ${SERVER_URL}`);
    process.exit(1);
  }

  // If system message provided, show it
  if (systemMessage) {
    console.log(`System message: ${systemMessage}\n`);
  }

  // If initial message provided, send it and exit
  if (initialUserMessage) {
    await handleChat(initialUserMessage);
    rl.close();
    return;
  }

  // Interactive mode
  rl.prompt();

  rl.on('line', async (input) => {
    const trimmed = input.trim();
    
    if (trimmed === 'exit' || trimmed === 'quit') {
      console.log('Goodbye!');
      rl.close();
      return;
    }
    
    if (trimmed === 'clear') {
      conversationHistory.length = 0;
      conversationId = '';
      console.log('Conversation history cleared.\n');
      rl.prompt();
      return;
    }
    
    if (trimmed === 'history') {
      console.log('\nConversation History:');
      console.log('-'.repeat(60));
      conversationHistory.forEach((msg, idx) => {
        console.log(`${idx + 1}. [${msg.role}]: ${msg.content.substring(0, 100)}${msg.content.length > 100 ? '...' : ''}`);
      });
      console.log('-'.repeat(60) + '\n');
      rl.prompt();
      return;
    }
    
    await handleChat(trimmed);
  });

  rl.on('close', () => {
    process.exit(0);
  });
}

// Handle Ctrl+C gracefully
process.on('SIGINT', () => {
  console.log('\n\nGoodbye!');
  rl.close();
  process.exit(0);
});

// Run main function
main().catch((error) => {
  console.error('Fatal error:', error);
  process.exit(1);
});

